#include "MainComponent.h"
#include "PokeLookAndFeel.h"
#include "LauncherComponent.h"
#include "Utils.h"

MainContentComponent::MainContentComponent(const var &configJson) {
  lookAndFeel = new PokeLookAndFeel();
  setLookAndFeel(lookAndFeel);

  LookAndFeel::setDefaultLookAndFeel(lookAndFeel);

  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  launcher = new LauncherComponent(configJson);
  pageStack->pushPage(launcher, PageStackComponent::kTransitionNone);

  setSize(480, 272);
}

MainContentComponent::~MainContentComponent() {}

void MainContentComponent::paint(Graphics &g) {
  g.fillAll(Colours::white);
}

void MainContentComponent::resized() {
  auto bounds = getLocalBounds();
  pageStack->setBounds(bounds);
}
