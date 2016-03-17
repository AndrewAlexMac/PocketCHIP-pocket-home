#include "Main.h"
#include "MainComponent.h"
#include "Utils.h"

void WifiStatus::setConnectedAccessPoint(WifiAccessPoint *ap) {
  connected = ap != nullptr;
  connectedAccessPoint = ap;
}

void WifiStatus::setDisconnected() {
  setConnectedAccessPoint(nullptr);
}

void WifiStatus::populateFromJson(const var &json) {
  connectedAccessPoint = nullptr;
  connected = false;

  accessPoints.clear();

  for (const auto &apJson : *json.getArray()) {
    auto ap = new WifiAccessPoint();
    ap->ssid = apJson["name"];
    ap->signalStrength = apJson["strength"];
    ap->requiresAuth = apJson["auth"];
    accessPoints.add(ap);
  }
}

void BluetoothStatus::populateFromJson(const var &json) {
  devices.clear();

  for (const auto &btDevice : *json.getArray()) {
    auto device = new BluetoothDevice();
    device->name = btDevice["name"].toString();
    device->macAddress = btDevice["mac"].toString();
    device->connected = btDevice["connected"];
    device->paired = btDevice["paired"];
    devices.add(device);
  }
}

PageStackComponent &getMainStack() {
  return PokeLaunchApplication::get()->getMainStack();
}

WifiStatus &getWifiStatus() {
  return PokeLaunchApplication::get()->wifiStatus;
}

BluetoothStatus &getBluetoothStatus() {
  return PokeLaunchApplication::get()->bluetoothStatus;
}

PokeLaunchApplication::PokeLaunchApplication() {}

PokeLaunchApplication *PokeLaunchApplication::get() {
  return dynamic_cast<PokeLaunchApplication *>(JUCEApplication::getInstance());
}

const String PokeLaunchApplication::getApplicationName() {
  return ProjectInfo::projectName;
}

const String PokeLaunchApplication::getApplicationVersion() {
  return ProjectInfo::versionString;
}

bool PokeLaunchApplication::moreThanOneInstanceAllowed() {
  return false;
}

void PokeLaunchApplication::initialise(const String &commandLine) {
  StringArray args;
  args.addTokens(commandLine, true);

  auto configJson = var::null;

  auto flagIndex = args.indexOf("-c");
  if (flagIndex >= 0 && args.size() > flagIndex + 1) {
    auto configFile = absoluteFileFromPath(args[flagIndex + 1]);
    configJson = JSON::parse(configFile);
    if (!configJson) {
      std::cerr << "Could not read config file from " << configFile.getFullPathName() << std::endl;
      quit();
    }
  } else {
    std::cout << "Usage: PokeLaunch -c <config-file.json>" << std::endl;
    quit();
  }

  // Populate with dummy data
  {
    auto ssidListFile = absoluteFileFromPath("../../assets/wifi.json");
    wifiStatus.populateFromJson(JSON::parse(ssidListFile));

    auto deviceListFile = absoluteFileFromPath("../../assets/bluetooth.json");
    bluetoothStatus.populateFromJson(JSON::parse(deviceListFile));
  }

  mainWindow = new MainWindow(getApplicationName(), configJson);
}

void PokeLaunchApplication::shutdown() {
  // Add your application's shutdown code here..

  mainWindow = nullptr; // (deletes our window)
}

void PokeLaunchApplication::systemRequestedQuit() {
  // This is called when the app is being asked to quit: you can ignore this
  // request and let the app carry on running, or call quit() to allow the app to close.
  quit();
}

void PokeLaunchApplication::anotherInstanceStarted(const String &commandLine) {
  // When another instance of the app is launched while this one is running,
  // this method is invoked, and the commandLine parameter tells you what
  // the other instance's command-line arguments were.
}

PageStackComponent &PokeLaunchApplication::getMainStack() {
  return *dynamic_cast<MainContentComponent *>(mainWindow->getContentComponent())->pageStack;
}

PokeLaunchApplication::MainWindow::MainWindow(String name, const var &configJson)
: DocumentWindow(name, Colours::darkgrey, DocumentWindow::allButtons) {
  setUsingNativeTitleBar(true);
  setResizable(true, false);
  setContentOwned(new MainContentComponent(configJson), true);
  centreWithSize(getWidth(), getHeight());
  setVisible(true);
#if JUCE_LINUX
//  setTitleBarHeight(0);
//  setFullScreen(true);
#endif
}

void PokeLaunchApplication::MainWindow::closeButtonPressed() {
  // This is called when the user tries to close this window. Here, we'll just
  // ask the app to quit when this happens, but you can change this to do
  // whatever you need.
  JUCEApplication::getInstance()->systemRequestedQuit();
}

START_JUCE_APPLICATION(PokeLaunchApplication)
