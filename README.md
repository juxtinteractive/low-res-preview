# low-res-preview
Application for testing input syphon/spout streams at diffent simulated pixel densities. 

## Dependencies

- [openFrameworks 0.8.4](http://openframeworks.cc/)
- [ofxUI](https://github.com/rezaali/ofxUI)
    - ofxXmlSettings

for OSX

- [ofxSyphon](https://github.com/astellato/ofxSyphon)

for Windows

- [ofxSpout](https://github.com/Mat-Loz/ofxSpout)

## Project Setup
### xcode

1. First ensure you have installed openFrameworks and the required addons.
2. Checkout the repository.
3. Copy `Project.xconfig.DIST` to `Project.xconfig`. Open `Project.xconfig` and edit the `OF_PATH` to be the relative path to your OF 0.8.4 install relative to the current folder, also adjust the `#include` path
4. Open xcode (not the lowResPreview project). Select _Xcode > Prefrences_. Under Preferences Select _Locations_ then select _Source Trees_. Add a new items with the Name set to `OF_0_8_4` a Display Name of `openFrameworks 0.8.4` and the Path set to the absolute path to your OF 0.8.4 installation. 
5. Close Xcode.
6. Open the lowResPreview project.


### Visual Studio

TBD