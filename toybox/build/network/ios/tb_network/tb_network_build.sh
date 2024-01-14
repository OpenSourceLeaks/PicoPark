#!/bin/bash
 
# エラー検出用
set -e

#release
xcodebuild -project tb_network.xcodeproj -sdk iphoneos -configuration Release -arch armv7 -arch armv7s -arch arm64 clean build
xcodebuild -project tb_network.xcodeproj -sdk iphonesimulator -configuration Release -arch i386 -arch x86_64 clean build
xcrun -sdk iphoneos lipo -create build/Release-iphonesimulator/libtb_network.a build/Release-iphoneos/libtb_network.a -output ../../../../lib/ios/libtb_network.a

#debug
xcodebuild -project tb_network.xcodeproj -sdk iphoneos -configuration Debug -arch armv7 clean build
xcodebuild -project tb_network.xcodeproj -sdk iphonesimulator -configuration Debug -arch x86_64 clean build
xcrun -sdk iphoneos lipo -create build/Debug-iphonesimulator/libtb_network.a build/Debug-iphoneos/libtb_network.a -output ../../../../lib/ios/libtb_networkd.a



