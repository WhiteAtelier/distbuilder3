// This file contains code generated with the assistance of Codex (OpenAI), an AI assistant.
// The generated code is provided as-is.

def runCommand(String command) {
    if (isUnix()) {
        sh command
    } else {
        bat command
    }
}

pipeline {
    agent any

    stages {
        stage('Configure') {
            steps {
                script {
                    runCommand('cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DDISTBUILDER3_BUILD_TESTS=ON')
                }
            }
        }

        stage('Build') {
            steps {
                script {
                    runCommand('cmake --build build --parallel')
                }
            }
        }

        stage('Test') {
            steps {
                script {
                    runCommand("ctest --test-dir build --output-on-failure --no-tests=error --output-junit \"${env.WORKSPACE}/build/test-results.xml\"")
                }
            }
            post {
                always {
                    junit testResults: 'build/test-results.xml'
                }
            }
        }

        stage('Install') {
            steps {
                dir('install') {
                    deleteDir()
                }
                script {
                    runCommand("cmake --install build --prefix \"${env.WORKSPACE}/install\"")
                }
                archiveArtifacts artifacts: 'install/**', fingerprint: true
            }
        }
    }
}
