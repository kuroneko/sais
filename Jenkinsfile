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
                            security unlock-keychain appsigning.keychain -p ''
                            codesign --keychain appsigning -s "AA22B289D7BCD9C66BC95F2AD2E8B35D8D1E4E7C" --timestamp "bin/Strange Adventures in Infinite Space.app"
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
