/*
==============================================================================

  This file was auto-generated by the Introjucer!

  It contains the basic startup code for a Juce application.

==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
PitchedDelayAudioProcessorEditor::PitchedDelayAudioProcessorEditor (PitchedDelayAudioProcessor* ownerFilter)
  : AudioProcessorEditor (ownerFilter),
    tabs(TabbedButtonBar::TabsAtTop, ownerFilter),
	addPreset("+", "Add new preset"),
	removePreset("-", "Remove current preset from list")
{
	LookAndFeel::setDefaultLookAndFeel(&lookAndFeel);

	addAndMakeVisible(&tabs);
	tabs.setTabBarDepth(25);

	for (int i=0; i<NUMDELAYTABS; ++i)
	{
		const String tabName("Delay Tap " + String(i+1));
		PitchedDelayTab* delay = new PitchedDelayTab(getProcessor(), i);
		delays.add(delay);
		tabs.addTab(tabName, Colour(0xFFC0C0C0), delays.getLast(), false);
		delay->addActionListener(this);
	}

	addAndMakeVisible(graph = new DelayGraph(delays, ownerFilter));

	addAndMakeVisible(&dryVolume);
	dryVolume.setRange(-60, 12, 0.1);
	dryVolume.setSliderStyle(Slider::LinearVertical);
	dryVolume.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
	dryVolume.addListener(this);

	addAndMakeVisible(&masterVolume);
	masterVolume.setRange(-60, 12, 0.1);
	masterVolume.setSliderStyle(Slider::LinearVertical);
	masterVolume.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
	masterVolume.addListener(this);

	addAndMakeVisible(&showTooltips);
	showTooltips.setButtonText("Show tooltips");
	showTooltips.addListener(this);

	addAndMakeVisible(&addPreset);
	addPreset.addListener(this);
	addAndMakeVisible(&removePreset);
	removePreset.addListener(this);

	addAndMakeVisible(&presetList);
	presetList.addListener(this);

	tooltipWindow = new TooltipWindow();

	{
#if LINUX
		File presetFile("~/.config/lkjb/PitchedDelayPresets.xml");
#else
		File presetFile(File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile("lkjb").getChildFile("PitchedDelay").getChildFile("presets.xml"));
#endif

		if (! presetFile.existsAsFile())
		{
			presetFile.getParentDirectory().createDirectory();
			presetFile.replaceWithText(String(BinaryData::factorypresets_xml, BinaryData::factorypresets_xmlSize));
		}

		presetManager = new PresetManager(ownerFilter, presetFile);
		updatePresets();
	}

	setSize (600, 435);

	startTimer(100);
}

PitchedDelayAudioProcessorEditor::~PitchedDelayAudioProcessorEditor()
{

}

void PitchedDelayAudioProcessorEditor::resized()
{
	addPreset.setBounds(getWidth() - 60, 0, 30, 20);
	removePreset.setBounds(getWidth() - 30, 0, 30, 20);
	presetList.setBounds(getWidth() - 360, 0, 300, 20);

	graph->setBounds(0, 20, 600, 100);
	tabs.setBounds(0, 120, 500, 315);
	dryVolume.setBounds(502, 140, 46, 265);
	masterVolume.setBounds(552, 140, 46, 265);
	showTooltips.setBounds(10, 0, 100, 20);
}

//==============================================================================
void PitchedDelayAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll (Colour(0xFFB0B0B0));
	g.setFont(Font(14.f));
	g.setColour(Colours::black);
	g.drawText("Dry", 500, 120, 50, 20, Justification::centred, false);
	g.drawText("Master", 550, 120, 50, 20, Justification::centred, false);

	g.drawText("Presets", 0, 0, getWidth()-370, 20, Justification::centredRight, false);
}

void PitchedDelayAudioProcessorEditor::timerCallback()
{
	PitchedDelayAudioProcessor* Proc = getProcessor();

	const int currentTab = Proc->currentTab;

	if (currentTab != tabs.getCurrentTabIndex() && currentTab >= 0)
		tabs.setCurrentTabIndex(currentTab, false);


	dryVolume.setValue(12. + Decibels::gainToDecibels(Proc->getParameter(Proc->getNumDelayParameters() + PitchedDelayAudioProcessor::kDryVolume)));
	masterVolume.setValue(12. + Decibels::gainToDecibels(Proc->getParameter(Proc->getNumDelayParameters() + PitchedDelayAudioProcessor::kMasterVolume)));

	showTooltips.setToggleState(Proc->showTooltips, dontSendNotification);

	if (Proc->showTooltips && tooltipWindow == nullptr)
		tooltipWindow = new TooltipWindow();
	else if (! Proc->showTooltips && tooltipWindow != nullptr)
		tooltipWindow = 0;

	for (int i=0; i<tabs.getNumTabs(); ++i)
	{
		const String tabName(String(Proc->getDelay(i)->isEnabled() ? "*" : "") + "Delay Tap " + String(i+1));
		tabs.setTabName(i, tabName);
	}
}

void PitchedDelayAudioProcessorEditor::actionListenerCallback(const String& message)
{
	PitchedDelayAudioProcessor* Proc = getProcessor();

	StringArray cmds;
	const int numCmds = cmds.addTokens(message, ":", "");

	if (numCmds == 3 && cmds[0].startsWith("Tab"))
	{		
		const String tab(cmds[0]);
		const String param(cmds[1]);
		const String val(cmds[2]);

		const int delayIdx = tab.substring(3).getIntValue();
		const double value = val.getDoubleValue();
		int paramIdx = -1;

		DelayTabDsp* dsp = Proc->getDelay(delayIdx);
		jassert(dsp != 0);

		if (param == "Delay")
		{
			paramIdx = DelayTabDsp::kDelay;
		}
		else if (param == "Sync")
		{
			paramIdx = DelayTabDsp::kSync;
		}
		else if (param == "Pitch")
		{
			paramIdx = DelayTabDsp::kPitch;
		}
		else if (param == "PitchType")
		{
			paramIdx = DelayTabDsp::kPitchType;
		}
		else if (param == "Feedback")
		{
			paramIdx = DelayTabDsp::kFeedback;
		}
		else if (param == "EqFreq")
		{
			paramIdx = DelayTabDsp::kFilterFreq;
		}
		else if (param == "EqQ")
		{
			paramIdx = DelayTabDsp::kFilterQ;
		}
		else if (param == "EqGain")
		{
			paramIdx = DelayTabDsp::kFilterGain;
		}
		else if (param == "Volume")
		{
			paramIdx = DelayTabDsp::kVolume;
		}
		else if (param == "Pan")
		{
			paramIdx = DelayTabDsp::kPan;
		}
		else if (param == "EqType")
		{
			paramIdx = DelayTabDsp::kFilterType;
		}
		else if (param == "PrePitch")
		{
			paramIdx = DelayTabDsp::kPrePitch;
		}
		else if (param == "Enabled")
		{
			paramIdx = DelayTabDsp::kEnabled;
		}
		else if (param == "Mode")
		{
			paramIdx = DelayTabDsp::kMode;
		}
		else if (param == "Predelay")
		{
			paramIdx = DelayTabDsp::kPreDelay;
		}
		else if (param == "PredelayVol")
		{
			paramIdx = DelayTabDsp::kPreDelayVol;
		}
		else
		{
			jassertfalse;
		}

		if (paramIdx >= 0)
		{
			const int procParem = paramIdx + delayIdx * DelayTabDsp::kNumParameters;
			Proc->setParameterNotifyingHost(procParem, dsp->plainToNormalized(paramIdx, value));
		}

	}
}

void PitchedDelayAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
	PitchedDelayAudioProcessor* Proc = getProcessor();

	if (slider == &dryVolume)
	{
		Proc->setParameterNotifyingHost(Proc->getNumDelayParameters() + PitchedDelayAudioProcessor::kDryVolume, Decibels::decibelsToGain((float) dryVolume.getValue() - 12.f));
	}
	else if (slider == &masterVolume)
	{
		Proc->setParameterNotifyingHost(Proc->getNumDelayParameters() + PitchedDelayAudioProcessor::kMasterVolume, Decibels::decibelsToGain((float) masterVolume.getValue() - 12.f));
	}
}

void PitchedDelayAudioProcessorEditor::buttonClicked (Button* button)
{
	if (button == &showTooltips)
	{
		getProcessor()->showTooltips = showTooltips.getToggleState();
	}
	else if (button == &addPreset)
	{
		AlertWindow aw("Add Preset", "Add preset", AlertWindow::QuestionIcon, this);
		aw.addTextEditor("name", presetList.getText(), "Preset name");
		aw.addButton("OK", 1, KeyPress(KeyPress::returnKey));
		aw.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));

		if (aw.runModalLoop() == 1)
		{
			const String presetName(aw.getTextEditor("name")->getText());
			StringArray presetNames(presetManager->getPresetNames());
			
			if (presetName.isEmpty())
			{
				AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Invalid name", "No Preset name specified", "OK");
				return;
			}

			if (presetNames.contains(presetName))
			{
				if (! AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon, "Overwrite Preset", "Overwrite " + presetName.quoted() + "?", "Overwrite", "Cancel", this))
					return;
			}

			presetManager->storePreset(presetName);

			updatePresets();
		}
	}
	else if (button == &removePreset)
	{
		const String presetName(presetList.getText());

		if (presetName.isEmpty())
			return;

		if (! AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon, "Remove Preset", "Remove " + presetName.quoted() + "?", "Remove", "Cancel", this))
			return;

		presetManager->removePreset(presetName, true);
		updatePresets();
	}
}

void PitchedDelayAudioProcessorEditor::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &presetList)
	{
		const String presetName(presetList.getText());
		presetManager->loadPreset(presetName);		
	}
}

void PitchedDelayAudioProcessorEditor::updatePresets()
{
	StringArray presets(presetManager->getPresetNames());
	presets.sort(true);

	presetList.clear();

	for (int i=0; i<presets.size(); ++i)
		presetList.addItem(presets[i], i+1);

	presetList.setSelectedId(0);
}
