#!/bin/bash
 
# エラー検出用
set -e

#release
xcodebuild -project tb_graphics.xcodeproj -sdk iphoneos -configuration Release -arch armv7 -arch armv7s -arch arm64 clean build
xcodebuild -project tb_graphics.xcodeproj -sdk iphonesimulator -configuration Release -arch i386 -arch x86_64 clean build
xcrun -sdk iphoneos lipo -create build/Release-iphonesimulator/libtb_graphics.a build/Release-iphoneos/libtb_graphics.a -output ../../../../lib/ios/libtb_graphics.a

#debug
xcodebuild -project tb_graphics.xcodeproj -sdk iphoneos -configuration Debug -arch armv7 clean build
xcodebuild -project tb_graphics.xcodeproj -sdk iphonesimulator -configuration Debug -arch x86_64 clean build
xcrun -sdk iphoneos lipo -create build/Debug-iphonesimulator/libtb_graphics.a build/Debug-iphoneos/libtb_graphics.a -output ../../../../lib/ios/libtb_graphicsd.a



