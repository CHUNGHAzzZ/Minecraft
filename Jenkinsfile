def scriptDir
def ROOT
def MAINPROJECT_DIRgit 
def OUTPUT_DIR
def BUILD_TYPE
def PRODUCT_NAME
def qt_tool
def windeployqtpath
def windeployqt_qml_path
def commitHash
def scmVars

pipeline {
    agent {
        label 'VHS-SZL-0051'
    }

    stages {
        stage('initialize') {
            steps {
                script {
                    scriptDir = "${WORKSPACE}\\script"
                    ROOT = "${WORKSPACE}"
                    MAINPROJECT_DIR = "${WORKSPACE}"
                    OUTPUT_DIR = "${WORKSPACE}\\output"
                    BUILD_TYPE = 'Release'
                    PRODUCT_NAME = 'CHITUBOX_Pro'
                    qt_tool = 'git@gitlab.chuangbide.com:redrabbit/3rdparty/libqt.git'
                    windeployqtpath = "C:\\Qt\\5.15.2\\msvc2019_64\\bin\\windeployqt.exe"
                    windeployqt_qml_path = "C:\\Qt\\5.15.2\\msvc2019_64\\qml"
                    scmVars = checkout([$class: 'GitSCM', branches: [[name: 'qt5_15_2']], 
                        userRemoteConfigs: [[url: 'git@gitlab.chuangbide.com:redrabbit/redrabbitproject.git']]])
                    commitHash = scmVars.GIT_COMMIT
                }
            }
        }
        stage('checkout') {
            steps {
                bat 'git clean -dxf .'
                bat 'git restore .'
                bat 'git fetch --all'
                bat 'git switch qt5_15_2'
                // bat 'git pull'
                bat 'git reset --hard origin/qt5_15_2'
                bat 'git submodule update --init --recursive'
            }
        }
        stage('build') {
            steps {
                echo "dir: ${workspace}"
                bat """
                    powershell.exe If (!(test-path ${OUTPUT_DIR})) { md ${OUTPUT_DIR} }
                    """
                bat "powershell.exe \"${MAINPROJECT_DIR}\\build\\win_build.bat | Out-File -Encoding utf8 -FilePath ${OUTPUT_DIR}\\build.log\""
            }
        }
        // stage('test') {
        //     agent {
        //         label 'VHS-SZL-0051'
        //     }
        //     steps {
        //         bat 'echo test'
        //         // TODO
        //     }
        // }
        stage('deploy') {
            steps {
                bat "powershell.exe ${MAINPROJECT_DIR}\\build\\win_resources_copy.bat"
                bat "${windeployqtpath} -qmldir ${windeployqt_qml_path} --release ${MAINPROJECT_DIR}\\build\\RedRabbitSymbols\\win\\bin\\${BUILD_TYPE}\\${PRODUCT_NAME}.exe"
            }
        }

        stage('pack') {
            steps {
                bat "\"C:\\Program Files (x86)\\NSIS\\makensis.exe\" /INPUTCHARSET UTF8 ${scriptDir}\\installer.nsi > ${OUTPUT_DIR}\\pack.log 2>&1"
            }
        }
    }

    post {
        success {
            echo "success"
            archiveArtifacts artifacts: "build\\RedRabbitSymbols\\win\\bin\\Release\\${PRODUCT_NAME}.exe, script\\ChituDentalInstaller.exe", fingerprint: true
            bat 'powershell pwd'
            bat """powershell
                   if exist installer rd /s /q installer
                """
            bat 'git clone git@gitlab.chuangbide.com:redrabbit/redrabbitbin/installer.git'
            bat """powershell
                    cd ./installer
                    git switch master
                    cd ./win64
                    echo F | xcopy /s /i /Y ..\\..\\script\\ChituDentalInstaller.exe .
                    git add -A
                    git commit -m "Hash: ${commitHash}"
                    git push
                """

            qyWechatNotification failNotify: true, mentionedId: '', webhookUrl: 'https://qyapi.weixin.qq.com/cgi-bin/webhook/send?key=3afe1a29-9eb8-4d44-a6f8-1ddb2f2fb114', moreInfo:"commit hash: ${commitHash}</font>"
        }
        failure{
            // bat 'echo failed'
            qyWechatNotification failNotify: true, mentionedId: '', webhookUrl: 'https://qyapi.weixin.qq.com/cgi-bin/webhook/send?key=3afe1a29-9eb8-4d44-a6f8-1ddb2f2fb114', moreInfo:"commit hash: ${commitHash}</font>"
        }
    }
}

// @NonCPS
// String getCommitMessage(){
//     commitMessage = " "
//     for ( changeLogSet in currentBuild.changeSets){
//         for (entry in changeLogSet.getItems()){
//             commitMessage = entry.msg
//         }
//     }
//     return commitMessage
// }