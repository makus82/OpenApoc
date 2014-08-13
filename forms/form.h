
#pragma once

#include "control.h"

class Form : public Control
{

	protected:
		virtual void OnRender();

	public:
		Form( tinyxml2::XMLElement* FormConfiguration );
		virtual ~Form();

		virtual void EventOccured( Event* e );
		virtual void Update();
		virtual void UnloadResources();
};
