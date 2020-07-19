pipeline {
    agent none
    options {
        checkoutToSubdirectory('saisgpl')
        disableConcurrentBuilds()
    }
    stages {
        stage('build') {
            parallel {
                stage('linux64') {
                    agent {
                        label 'linux'
                    }
                    steps {
                        dir('saisgpl.l64') {
                            deleteDir()
                            sh '''
                            conan install ../saisgpl  --build=outdated --build=cascade --update --profile=default
                            '''
                        }
                        cmakeBuild generator: 'Ninja',
                            buildDir: 'saisgpl.l64',
                            sourceDir: 'saisgpl',
                            cmakeArgs: "-DUSE_CONAN=ON",
                            buildType: 'Release',
                            installation: 'CMake 3.16.0',
                            steps: [
                                [args: 'all']
                            ]
                        dir('saisgpl.l64') {
                            cpack installation: 'CMake 3.16.0'
                            archiveArtifacts artifacts: 'SAIS-GPL-**', defaultExcludes: false, fingerprint: true
                        }
                    }
                }
                stage('macos64') {
                    agent {
                        label 'macos'
                    }
                    steps {
                        dir('saisgpl.m64') {
                            deleteDir()
                            sh '''
                            export PATH=/usr/local/bin:$PATH
            			    export MACOS_DEPLOYMENT_TARGET=10.9
                            conan install ../saisgpl --build --update --profile=default
                            '''
                        }
                        cmakeBuild buildDir: 'saisgpl.m64',
                            sourceDir: 'saisgpl',
                            cmakeArgs: "-DUSE_CONAN=ON",
                            buildType: 'Release',
                            installation: 'CMake 3.16.0',
                            steps: [
                                [args: 'all']
                            ]
                        dir('saisgpl.m64') {
                            sh '''
                            set +x

                            security unlock-keychain -p "${APPSIGNING_PASSWORD}" appsigning
                            codesign --keychain appsigning -s "${APPSIGNING_KEYID}" --options runtime,library --timestamp "bin/Strange Adventures in Infinite Space.app"
                            '''
                            cpack installation: 'CMake 3.16.0',
                                arguments: '-G ZIP'
                            sh '''
                            set +x

                            PATH=/usr/local/bin:"${PATH}"
                            export PATH

                            xcrun altool --notarize-app  --primary-bundle-id "au.com.ecsim.SAISGPL" --username "${APPSIGNING_APPLEUSER}" --password "${APPSIGNING_APPLEPW}" --file SAIS-GPL-*-macOS-x86_64.zip --output-format xml | tee notarization-submission.plist
                            plutil -convert json notarization-submission.plist
                            REQUEST_ID="$(jq '."notarization-upload".RequestUUID' -r notarization-submission.plist)"
                            echo "Notarization Request ID: ${REQUEST_ID}"

                            NOTARIZATION_STATUS=""
                            update_status () {
                              xcrun altool --notarization-info "${REQUEST_ID}" --username "${APPSIGNING_APPLEUSER}" --password "${APPSIGNING_APPLEPW}" --output-format=xml > notarization-result.plist
                              plutil -remove 'notarization-info.Date' notarization-result.plist
                              plutil -convert json notarization-result.plist
                              NOTARIZATION_STATUS="$(jq '."notarization-info".Status' -r notarization-result.plist)"
                            }

                            update_status
                            echo "NOTARIZATION_STATUS is ${NOTARIZATION_STATUS}"
                            while [ "${NOTARIZATION_STATUS}" = "in progress" ]; do
                              sleep 60
                              update_status
                              echo "NOTARIZATION_STATUS is ${NOTARIZATION_STATUS}"
                            done
                            if [ "${NOTARIZATION_STATUS}" != "success" ]; then
                              echo "Notarization failed."
                              exit 1
                            fi
                            xcrun stapler staple -v "bin/Strange Adventures in Infinite Space.app"
                            '''
                            cpack installation: 'CMake 3.16.0'
                            archiveArtifacts artifacts: 'SAIS-GPL-**', defaultExcludes: false, fingerprint: true
                        }
                    }
                }
                stage('win64') {
                    agent {
                        label 'windows'
                    }
                    steps {
                        dir('saisgpl.w64') {
                            deleteDir()
                            bat '''
                            conan install ..\\saisgpl --build=outdated --build=cascade --update --profile=default -s compiler.runtime=MT
                            '''
                        }
                        cmakeBuild generator: 'Visual Studio 16 2019',
                            sourceDir: 'saisgpl',
                            buildDir: 'saisgpl.w64',
                            cmakeArgs: "-DUSE_CONAN=ON",
                            buildType: 'Release',
                            installation: 'CMake 3.16.0',
                            steps: [
                                [args: '-- -p:Configuration=Release', withCmake: true]
                            ]
                        dir('saisgpl.w64') {
                            cpack installation: 'CMake 3.16.0'
                            archiveArtifacts artifacts: 'SAIS-GPL-**', defaultExcludes: false, fingerprint: true
                        }
                    }
                }

            }
        }
    }
}
