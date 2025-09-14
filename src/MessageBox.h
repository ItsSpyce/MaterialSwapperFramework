#pragma once

class SkyrimMessageBox {
  class ResultCallback : public RE::IMessageBoxCallback {
    function<void(unsigned int)> callback_;

   public:
    ~ResultCallback() override = default;
    ResultCallback(const function<void(unsigned int)>& callback)
        : callback_(callback) {}
    void Run(Message msg) override {
      callback_(static_cast<unsigned int>(msg));
    }
  };
public:
  static void Show(const string& body, const vector<string>& buttons,
                   const function<void(unsigned int)>& callback) {
    auto* factoryManager = RE::MessageDataFactoryManager::GetSingleton();
    auto* uiStringHelper = RE::InterfaceStrings::GetSingleton();
    auto* factory = factoryManager->GetCreator<RE::MessageBoxData>(
        uiStringHelper->messageBoxData);
    auto* messageBox = factory->Create();
    RE::BSTSmartPointer<RE::IMessageBoxCallback> cb =
        RE::make_smart<ResultCallback>(callback);
    messageBox->callback = cb;
    messageBox->bodyText = body;
    for (auto& button : buttons) {
      messageBox->buttonText.push_back(button.c_str());
    }
    messageBox->QueueMessage();
  }
};

static void ShowMessageBox(
    const string& body, const vector<string>& buttons,
    const function<void(unsigned int)>& callback) {
  SkyrimMessageBox::Show(body, buttons, callback);
}
