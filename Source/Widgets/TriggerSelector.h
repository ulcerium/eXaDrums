/*
 * TriggerSelector.h
 *
 *  Created on: 14 Mar 2017
 *      Author: jeremy
 */

#ifndef SOURCE_WIDGETS_TRIGGERSELECTOR_H_
#define SOURCE_WIDGETS_TRIGGERSELECTOR_H_

#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>

#include <memory>
#include <string>

namespace Widgets
{

	class TriggerSelector : public Gtk::Grid
	{

	public:

		explicit TriggerSelector(int sensorId);
		virtual ~TriggerSelector() = default;


		int GetSensorId() const { return sensorId; }
		Gtk::Button& GetPreferencesButton() { return preferencesButton; }


	private:

		int sensorId;
		Gtk::Label label;
		Gtk::Button preferencesButton;

	};


	typedef std::shared_ptr<TriggerSelector> TriggerSelectorPtr;

} /* namespace Widgets */

#endif /* SOURCE_WIDGETS_TRIGGERSELECTOR_H_ */
