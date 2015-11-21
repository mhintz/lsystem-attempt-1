.PHONY: build run

all: build run

build:
	xcodebuild -configuration Debug -project xcode/lsystem.xcodeproj/

run:
	./xcode/build/Debug/lsystem.app/Contents/MacOS/lsystem
