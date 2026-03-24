#!/bin/sh
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SUBMODULE_NAME="ChituDental"
AppName="macos"
OUTPUT_DIR=$SCRIPT_DIR/$SUBMODULE_NAME/$AppName
FRAMEWORK_DIR=$SCRIPT_DIR/$SUBMODULE_NAME/$AppName/$SUBMODULE_NAME.app/Contents/Frameworks
EXE_DIR=$SCRIPT_DIR/$SUBMODULE_NAME/$AppName/$SUBMODULE_NAME.app/Contents/MacOS
RESOURCES=$OUTPUT_DIR/$SUBMODULE_NAME.app/Contents/Resources

echo $FRAMEWORK_DIR
echo $EXE_DIR
echo $RESOURCES

rm -rf $FRAMEWORK_DIR/rabbit_framework.framework/Headers
rm -rf $FRAMEWORK_DIR/upgrade.framework/Headers

ln -hfs Versions/Current/Resources $FRAMEWORK_DIR/rabbit_framework.framework/Resources
ln -hfs Versions/Current/Resources $FRAMEWORK_DIR/upgrade.framework/Resources
ln -hfs Versions/Current/rabbit_framework $FRAMEWORK_DIR/rabbit_framework.framework/rabbit_framework
ln -hfs Versions/Current/upgrade $FRAMEWORK_DIR/upgrade.framework/upgrade

mv $EXE_DIR/resources $RESOURCES
ln -s ../Resources/resources $EXE_DIR

codesign --timestamp --options=runtime -f -s "Developer ID Application: Shenzhen CBD Technology Co.,Ltd. (HVAMW62TDK)" -v $OUTPUT_DIR/$SUBMODULE_NAME.app --deep

/usr/local/bin/create-dmg --skip-jenkins --volname "$SUBMODULE_NAME" "$SUBMODULE_NAME.dmg" "$OUTPUT_DIR/$SUBMODULE_NAME.app"

codesign --timestamp --options=runtime -f -s "Developer ID Application: Shenzhen CBD Technology Co.,Ltd. (HVAMW62TDK)" -v $SUBMODULE_NAME.dmg --deep

xcrun altool --notarize-app --primary-bundle-id "com.chitubox.chitudental" --username "huangwenjie204@cbd-3d.com" --password "kdnj-gkve-wnyu-xqdm" --asc-provider "HVAMW62TDK" -t osx --file $SUBMODULE_NAME.dmg