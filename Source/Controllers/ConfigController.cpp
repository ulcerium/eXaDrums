/*
 * ConfigController.cpp
 *
 *  Created on: 9 Feb 2017
 *      Author: jeremy
 */

#include "ConfigController.h"

#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/entry.h>
#include <gtkmm/box.h>

#include <iostream>
#include <cstring>

using namespace eXaDrumsApi;
using namespace Widgets;

namespace Controllers
{

	ConfigController::ConfigController(Glib::RefPtr<Gtk::Builder> builder, std::shared_ptr<eXaDrums> drumKit)
	: builder(builder), drumKit(drumKit), config(*drumKit.get())
	{

		Gtk::Button* mixerConfigButton = nullptr;
		Gtk::Button* soundLibraryButton = nullptr;

		// Triggers
		Gtk::Button* triggersConfigButton = nullptr;
		Gtk::Button* triggersSelectCloseButton = nullptr;
		Gtk::Button* triggerConfigCancelButton = nullptr;
		Gtk::Button* triggerConfigApplyButton = nullptr;
		Gtk::Button* triggerConfigSaveButton = nullptr;

		Gtk::Button* soundEffectsButton = nullptr;

		// Sensors
		Gtk::Button* sensorsConfigButton = nullptr;
		Gtk::Button* sensorsConfigOkayButton = nullptr;
		Gtk::Button* sensorsConfigCancelButton = nullptr;

		// Mixer
		Gtk::Button* mixerConfigCancelButton = nullptr;
		Gtk::Button* mixerConfigSaveButton = nullptr;

		// Get all widgets
		{

			// Buttons
			builder->get_widget("MixerConfigButton", mixerConfigButton);
			builder->get_widget("SoundLibraryButton", soundLibraryButton);

			// Triggers
			builder->get_widget("TriggersConfigButton", triggersConfigButton);
			builder->get_widget("TriggerSelectCloseButton", triggersSelectCloseButton);
			builder->get_widget("TriggerConfigCancel", triggerConfigCancelButton);
			builder->get_widget("TriggerConfigApply", triggerConfigApplyButton);
			builder->get_widget("TriggerConfigSave", triggerConfigSaveButton);


			builder->get_widget("SoundEffectsButton", soundEffectsButton);

			// Sensors
			builder->get_widget("SensorsConfigButton", sensorsConfigButton);
			builder->get_widget("SensorsConfigCancelButton", sensorsConfigCancelButton);
			builder->get_widget("SensorsConfigOkayButton", sensorsConfigOkayButton);

			// Mixer
			builder->get_widget("MixerConfigCancel", mixerConfigCancelButton);
			builder->get_widget("MixerConfigSave", mixerConfigSaveButton);

			// Comboboxes
			builder->get_widget("MixerDevices", mixerDevices);

			// Windows
			builder->get_widget("SensorsConfigWindow", sensorsConfigWindow);
			builder->get_widget("TriggerSeclectWindow", triggerSelectWindow);
			builder->get_widget("TriggerConfigurationWindow", triggerConfigWindow);
			builder->get_widget("MixerConfigWindow", mixerConfigWindow);

		}

		// Connect signals
		{

			mixerConfigButton->signal_clicked().connect([&] { ShowMixerConfigWindow(); });
			soundLibraryButton->signal_clicked().connect([&] { ShowSoundLibConfigWindow(); });
			soundEffectsButton->signal_clicked().connect([&] { ShowSoundEffectsWindow(); });
			sensorsConfigCancelButton->signal_clicked().connect([&] { sensorsConfigWindow->hide(); });

			// Triggers config
			triggersConfigButton->signal_clicked().connect([&] { ShowTriggersConfigWindow(); });
			triggersSelectCloseButton->signal_clicked().connect([&] { CloseTriggerSelectWindow(); });
			triggerConfigCancelButton->signal_clicked().connect([&] { CloseTriggerConfigWindow(); });
			triggerConfigApplyButton->signal_clicked().connect([&] { ApplyTriggerConfig(); });
			triggerConfigSaveButton->signal_clicked().connect([&] { SaveTriggerConfig(); });

			// Sensors config
			sensorsConfigButton->signal_clicked().connect([&] { ShowSensorsConfigWindow(); });
			sensorsConfigOkayButton->signal_clicked().connect([&] { SaveSensorsConfig(); });

			// Mixer config
			mixerConfigCancelButton->signal_clicked().connect([&] { mixerConfigWindow->hide(); });
			mixerConfigSaveButton->signal_clicked().connect([&] { SaveMixerConfig(); });

		}

		// Configure sensors window
		{

			// Sampling rate
			Gtk::Entry* samplingRate = nullptr;
			builder->get_widget("SensorsSamplingRate", samplingRate);

			int sRate = config.GetSensorsSamplingRate();
			samplingRate->set_text(std::to_string(sRate));

			// Resolution
			Gtk::Entry* resolution = nullptr;
			builder->get_widget("SensorsResolution", resolution);

			int res = config.GetSensorsResolution();
			resolution->set_text(std::to_string(res));

			// Sensor type
			Gtk::ComboBoxText* sensorsTypesList = nullptr;
			builder->get_widget("SensorsTypes", sensorsTypesList);

			std::vector<std::string> sensorsTypes = config.GetSensorsTypes();

			// Append values to combobox
			std::for_each(sensorsTypes.cbegin(), sensorsTypes.cend(), [&](const std::string& s) { sensorsTypesList->append(s); });

			const std::string selectedType = config.GetSensorsType();
			sensorsTypesList->set_active_text(selectedType);

			Gtk::Entry* hddDataFolder = nullptr;
			builder->get_widget("SensorsDataFolder", hddDataFolder);
			std::string dataFolder = config.GetSensorsDataFolder();
			hddDataFolder->set_text(dataFolder);

		}

		// Configure triggers window
		{
			// Triggers types
			Gtk::ComboBoxText* types = nullptr;
			builder->get_widget("TCTypes", types);

			std::vector<std::string> typesVec = config.GetTriggersTypes();
			std::for_each(typesVec.cbegin(), typesVec.cend(), [&](const std::string& s) { types->append(s); });

			// Triggers responses
			Gtk::ComboBoxText* responses = nullptr;
			builder->get_widget("TCResponses", responses);

			std::vector<std::string> responsesVec = config.GetTriggersResponses();
			std::for_each(responsesVec.cbegin(), responsesVec.cend(), [&](const std::string& s) { responses->append(s); });


		}



		return;
	}


	ConfigController::~ConfigController()
	{

		delete sensorsConfigWindow;
		delete triggerSelectWindow;
		delete triggerConfigWindow;
		delete mixerConfigWindow;

		return;
	}

	// Private Methods

	void ConfigController::ShowMixerConfigWindow()
	{

		Gtk::Button* playButton = nullptr;
		Gtk::Entry* sampleRate = nullptr;
		Gtk::Entry* bufferLength = nullptr;
		Gtk::Entry* periodLength = nullptr;
		Gtk::Entry* nbChannels = nullptr;

		builder->get_widget("PlayButton", playButton);
		builder->get_widget("MixerSamplingRate", sampleRate);
		builder->get_widget("MixerBufferLength", bufferLength);
		builder->get_widget("MixerPeriodLength", periodLength);
		builder->get_widget("MixerNbChannels", nbChannels);

		if(drumKit->IsStarted())
		{
			playButton->set_property("label", Gtk::StockID("gtk-media-play"));
			drumKit->Stop();
		}

		// Configure mixer window
		{
			mixerDevices->unset_active();
			mixerDevices->remove_all();

			// Get sound devices
			std::vector<std::string> devices = config.GetAudioDevicesNames();

			for(const auto& device : devices)
			{
				mixerDevices->append(device);
			}

			// Get selected device
			std::string devName = config.GetAudioDeviceName();
			mixerDevices->set_active_text(devName);

			AlsaParamsApi alsaParameters = config.GetAudioDeviceParams();

			sampleRate->set_text(std::to_string(alsaParameters.sampleRate));
			bufferLength->set_text(std::to_string(alsaParameters.bufferTime));
			periodLength->set_text(std::to_string(alsaParameters.periodTime));
			nbChannels->set_text(std::to_string(alsaParameters.nChannels));

		}

		mixerConfigWindow->show();

		return;
	}

	void ConfigController::SaveMixerConfig()
	{

		Gtk::Entry* sampleRate = nullptr;
		Gtk::Entry* bufferLength = nullptr;
		Gtk::Entry* periodLength = nullptr;
		Gtk::Entry* nbChannels = nullptr;

		builder->get_widget("MixerSamplingRate", sampleRate);
		builder->get_widget("MixerBufferLength", bufferLength);
		builder->get_widget("MixerPeriodLength", periodLength);
		builder->get_widget("MixerNbChannels", nbChannels);

		AlsaParamsApi alsaParameters;
		alsaParameters.sampleRate = std::stoi(sampleRate->get_text());
		alsaParameters.bufferTime = std::stoi(bufferLength->get_text());
		alsaParameters.periodTime = std::stoi(periodLength->get_text());
		alsaParameters.nChannels = std::stoi(nbChannels->get_text());

		std::strcpy(alsaParameters.device, mixerDevices->get_active_text().data());


		config.SaveAudioDeviceConfig(alsaParameters);
		config.ResetAudioDevice();


		mixerConfigWindow->hide();

		return;
	}

	void ConfigController::ShowSoundLibConfigWindow()
	{

		return;
	}

	void ConfigController::ShowTriggersConfigWindow()
	{

		Gtk::Box* triggersBox = nullptr;
		builder->get_widget("TSListBox", triggersBox);

		const std::vector<TriggerParameters>& triggersParameters = config.GetTriggersParameters();

		//std::for_each(triggersSelectors.begin(), triggersSelectors.end(), [](TriggerSelectorPtr& t) { t.reset(); });
		triggersSelectors.clear();

		for(const auto& t : triggersParameters)
		{
			triggersSelectors.push_back(std::make_shared<TriggerSelector>(t.sensorId));
		}

		std::for_each(triggersSelectors.cbegin(), triggersSelectors.cend(), [&](const TriggerSelectorPtr& t){ triggersBox->add(*t); });

		// Connect signals
		for(const auto& ts : triggersSelectors)
		{
			ts->GetPreferencesButton().signal_clicked().connect([&] { TriggerConfiguration(ts->GetSensorId()); });
		}


		triggerSelectWindow->show();

		return;
	}

	void ConfigController::ApplyTriggerConfig()
	{


		// Create pointers
		Gtk::Entry* sensorNb = nullptr;
		Gtk::Entry* threshold = nullptr;
		Gtk::Entry* scanTime = nullptr;
		Gtk::Entry* maskTime = nullptr;
		Gtk::ComboBoxText* types = nullptr;
		Gtk::ComboBoxText* responses = nullptr;

		// Get widgets
		{
			builder->get_widget("TCSensorNb", sensorNb);
			builder->get_widget("TCThreshold", threshold);
			builder->get_widget("TCScanTime", scanTime);
			builder->get_widget("TCMaskTime", maskTime);
			builder->get_widget("TCTypes", types);
			builder->get_widget("TCResponses", responses);
		}

		int sensorIdValue = std::stoi(sensorNb->get_text());
		int thresholdValue = std::stoi(threshold->get_text());

		return;
	}

	void ConfigController::CloseTriggerSelectWindow()
	{

		triggerSelectWindow->hide();

		return;
	}

	void ConfigController::CloseTriggerConfigWindow()
	{

		config.LoadTriggersConfig();
		config.SaveTriggersConfig();

		triggerConfigWindow->hide();
		triggerSelectWindow->show();

		return;
	}

	void ConfigController::SaveTriggerConfig()
	{


		// Create pointers
		Gtk::Entry* sensorNb = nullptr;
		Gtk::Entry* threshold = nullptr;
		Gtk::Entry* scanTime = nullptr;
		Gtk::Entry* maskTime = nullptr;
		Gtk::ComboBoxText* types = nullptr;
		Gtk::ComboBoxText* responses = nullptr;

		// Get widgets
		{
			builder->get_widget("TCSensorNb", sensorNb);
			builder->get_widget("TCThreshold", threshold);
			builder->get_widget("TCScanTime", scanTime);
			builder->get_widget("TCMaskTime", maskTime);
			builder->get_widget("TCTypes", types);
			builder->get_widget("TCResponses", responses);
		}

		TriggerParameters tp;
		tp.sensorId = std::stoi(sensorNb->get_text());
		tp.threshold = std::stoi(threshold->get_text());
		tp.scanTime = std::stoi(scanTime->get_text());
		tp.maskTime = std::stoi(maskTime->get_text());

		std::strcpy(tp.type, types->get_active_text().c_str());
		std::strcpy(tp.response, responses->get_active_text().c_str());

		// Get triggers parameters
		config.LoadTriggersConfig();
		std::vector<TriggerParameters> triggersParameters = config.GetTriggersParameters();

		// Find the trigger we want to modify
		auto triggerIt = std::find_if(triggersParameters.begin(), triggersParameters.end(), [&](TriggerParameters& t) { return t.sensorId == tp.sensorId; });

		if(triggerIt == triggersParameters.end())
		{
			// Error: trigger doesn't exist.
			std::cerr << "Error: trigger " << tp.sensorId << " doesn't exist." << std::endl;
			throw -1;
		}

		// Update parameters
		*triggerIt = tp;

		config.SetTriggersParameters(triggersParameters);

		// Save
		config.SaveTriggersConfig();

		triggerConfigWindow->hide();
		triggerSelectWindow->show();

		return;
	}



	void ConfigController::ShowSoundEffectsWindow()
	{

		return;
	}

	void ConfigController::ShowSensorsConfigWindow()
	{

		sensorsConfigWindow->show();

		return;
	}

	void ConfigController::SaveSensorsConfig()
	{


		Gtk::Entry* samplingRate = nullptr;
		Gtk::Entry* resolution = nullptr;
		Gtk::ComboBoxText* sensorsTypesList = nullptr;
		Gtk::Entry* hddDataFolder = nullptr;

		builder->get_widget("SensorsSamplingRate", samplingRate);
		builder->get_widget("SensorsResolution", resolution);
		builder->get_widget("SensorsTypes", sensorsTypesList);
		builder->get_widget("SensorsDataFolder", hddDataFolder);

		int sRate = std::stoi(samplingRate->get_text());
		int res = std::stoi(resolution->get_text());
		std::string type = sensorsTypesList->get_active_text();
		std::string dataFolder = hddDataFolder->get_text();


		config.SetSensorsSamplingRate(sRate);
		config.SetSensorsResolution(res);
		config.SetSensorsType(type);
		config.SetSensorsDataFolder(dataFolder);

		config.SaveSensorsConfig();

		sensorsConfigWindow->hide();

		return;
	}


	void ConfigController::TriggerConfiguration(int sensorId)
	{


		triggerSelectWindow->hide();

		// Create pointers
		Gtk::Entry* sensorNb = nullptr;
		Gtk::Entry* threshold = nullptr;
		Gtk::Entry* scanTime = nullptr;
		Gtk::Entry* maskTime = nullptr;
		Gtk::ComboBoxText* types = nullptr;
		Gtk::ComboBoxText* responses = nullptr;

		// Get widgets
		{
			builder->get_widget("TCSensorNb", sensorNb);
			builder->get_widget("TCThreshold", threshold);
			builder->get_widget("TCScanTime", scanTime);
			builder->get_widget("TCMaskTime", maskTime);
			builder->get_widget("TCTypes", types);
			builder->get_widget("TCResponses", responses);
		}

		const std::vector<TriggerParameters>& triggers = config.GetTriggersParameters();
		const TriggerParameters& trigger = triggers[sensorId];

		const std::string type(trigger.type);
		const std::string response(trigger.response);

		// Set fields values
		{
			sensorNb->set_text(std::to_string(trigger.sensorId));
			threshold->set_text(std::to_string(trigger.threshold));
			scanTime->set_text(std::to_string(trigger.scanTime));
			maskTime->set_text(std::to_string(trigger.maskTime));
			types->set_active_text(type);
			responses->set_active_text(response);
		}


		triggerConfigWindow->show();

		return;
	}



} /* namespace Controllers */
