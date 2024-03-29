/*
Copyright (c) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _GUI_MANAGER_H
#define _GUI_MANAGER_H

#include "prerequisites.h"
#include "fileParser.h"
#include "singleton.h"
#include "sticker.h"
#include "fontManager.h"
#include "inputManager.h"

namespace k
{
	/**
	 * \brief Signals type.
	 */
	typedef enum
	{
		SIGNAL_MOUSEIN,
		SIGNAL_MOUSEOUT,
		SIGNAL_MOUSEMOVE,
		SIGNAL_MOUSEDOWN,
		SIGNAL_MOUSEUP,
		SIGNAL_KEYDOWN,
		SIGNAL_KEYUP
	} signalType_t;

	/**
	 * Widgets states
	 */
	typedef enum
	{
		STATE_NORMAL = 0,
		STATE_HIGHLIGHTED,
		STATE_PRESSED
	} stateType_t;

	/**
	 * \brief Mouse/Pointer event params for gui signals.
	 */
	typedef enum
	{
		BUTTON_LEFT = (1 << 0),
		BUTTON_RIGHT = (1 << 1),
		BUTTON_WHEEL_UP = (1 << 2),
		BUTTON_WHEEL_DOWN = (1 << 3)
	} mouseParams_t;

	/**
	 * \brief Mouse structure for mouse/pointer signals.
	 */
	typedef struct
	{
		unsigned int buttons;
		vector2 mPosition;
	} mouseState_t;

	/**
	 * \brief Signal information.
	 */
	typedef struct
	{
		signalType_t type;
		mouseState_t mouseState;
		unsigned int key;
	} signalInfo_t;

	/**
	 * \brief A base class to make gui signal handlers
	 * This class describe a base class where you can derive
	 * your classes that will handle gui signal events.
	 */
	class DLL_EXPORT signalHandler {};

	typedef DLL_EXPORT bool(signalHandler::*signalFunctionPtr)(signalInfo_t, void*);

	/**
	 * \brief Signal Handle Information.
	 */
	typedef struct
	{
		// A pointer to the class instance holding the handle.
		signalHandler* handler;

		// A pointer to the handle itself.
		bool (signalHandler::*function)(signalInfo_t, void*);

		// Any user specified data.
		void* userData;
	} signalHandlerInfo_t;

	/**
	 * \brief A base class to store and use signals.
	 */
	class DLL_EXPORT signalKeeper
	{
		protected:
			std::map<std::string, signalHandlerInfo_t*> mInternalSignals;
			std::map<std::string, signalHandlerInfo_t*> mSignals;

		public:
			/**
			 * Destructor, deallocate all allocated signals
			 */
			virtual ~signalKeeper();

			/**
			 * Connect a signal to a handler.
			 * A signal is an event generated by the GUI and the handle is a function specified by the user to 
			 * handle the signal. Each signal can only have one handle and valid signals name are:
			 * "keyDown", "keyUp", "mouseIn", "mouseOut", "mouseMove". 
			 *
			 * User can also specify some data to be passed to the handle when the signal is called.
			 *
			 * @param sname The name of the signal to connect the handle function.
			 * @param info The signal handler information, @see signalHandlerInfo_t (this will be deallocated by the keeper).
			 */
			void connect(const std::string& sname, signalHandlerInfo_t* info);

			/**
			 * Disconnect a handle from a signal. The memory used by the handle will be free'd.
			 * @param sname The name of the signal.
			 */
			void disconnect(const std::string& sname);

			/**
			 * Call the signal handle by name.
			 * Returns true if the called has blocked the signal, that way the signal will stop
			 * its propagation to other widgets/handlers.
			 */
			bool callSignal(const std::string& sname, signalInfo_t info);
	};

	/**
	 * \brief The type of the widget skin. Following types are accepted:
	 * WIDGET_9 -> Use 9 images to make the skin, one for each corner and one for center.
	 * WIDGET_3 -> Use 3 images to make the skin, left, middle and right.
	 * WIDGET_1 -> Use a single image to make the skin, useful for checkboxes, etc.
	 */
	typedef enum
	{
		WIDGET_9,
		WIDGET_3,
		WIDGET_1,
	} widgetSkinType;
	
	/**
	 * \brief Shows the order of skin elements to be loaded
	 * and used on skin names array.
	 */
	typedef enum
	{
		E_TOPLEFT,
		E_TOPMIDDLE,
		E_TOPRIGHT,
		E_LEFT,
		E_MIDDLE,
		E_RIGHT,
		E_BOTTOMLEFT,
		E_BOTTOMMIDDLE,
		E_BOTTOMRIGHT,

		E_MAX_EDGES
	} skinEdges;

	/**
	 * \brief Hold skin information for a widget. 
	 * This class can hold a skinned area for widgets, allowing them
	 * to have multiple skins if desired. Each skin can be drawn by calling
	 * the draw() method. Keep in mind that you need to define the position
	 * and the drawing rectangle for each skin area.
	 */
	class DLL_EXPORT widgetSkin
	{
		protected:
			material* mSkinMaterial;
			vector2 mSkinDimensions;

			vector3* mVertices;
			vector2* mUvs;

		public:
			/**
			 * Create a new widget skin. Should be called
			 * on derivated widgets constructors. 
			 */
			widgetSkin()
			{
				mVertices = NULL;
				mUvs = NULL;
			}

			/**
			 * Free widget skin pointers.
			 */
			virtual ~widgetSkin()
			{
				if (mVertices)
					free(mVertices);

				if (mUvs)
					free(mUvs);
			}

			/**
			 * Draw the skin, remember that you need to specify
			 * the position and drawing area.
			 */
			virtual void drawWidgetSkin(const vector2& pos) = 0;

			/**
			 * Allocate and setup skin vertices.
			 */
			virtual void setSkinData(const char** skinArray, rectangle* parentRect) = 0;
	};

	/**
	 * \brief Just a base class to derive widgets from.
	 * Make sure you are calling widget constructor on your
	 * class constructor.
	 */
	class DLL_EXPORT widget : public signalKeeper, public drawable2D
	{
		protected:
			/**
			 * Is mouse in this widget?
			 */
			bool mMouseIn;

			/**
			 * All mouse buttons down on this widget.
			 */
			unsigned int mMouseButtons;

		public:
			widget()
			{
				mMouseIn = false;
				mMouseButtons = 0;
			}
			
			/**
			 * Tells if the pointer is inside this widget area.
			 */
			bool isPointerInside(const vector2& pos) const;
			
			/**
			 * Virtual mousemove method. Because we already used it to implement
			 * mouseIn and mouseOut.
			 */
			virtual bool mousemove(signalInfo_t info, void* userData);
	};

	/**
	 * \brief Panel skin names array.
	 */
	extern const char* DLL_EXPORT panelWidgetEdges[];	

	/**
	 * \brief Panel Skin.
	 */
	class DLL_EXPORT panelSkin : public widgetSkin
	{
		protected:
			rectangle* mSkin[E_MAX_EDGES];

		public:
			panelSkin() : widgetSkin() {}

			void setSkinData(const char** skinArray, rectangle* parentRect);
			void drawWidgetSkin(const vector2& pos);
	};

	/**
	 * \brief Panel widget, hold other widgets.
	 * The panel is basically a skinned box which hold another
	 * types of widgets within its area. Every widget contained in the
	 * panel will only be drawn if the panel is visible and will be free'd
	 * upon panel's deletion.
	 */
	class DLL_EXPORT panelWidget : public panelSkin, public widget, public signalHandler
	{
		protected:
			std::vector<drawable2D*> mChilds;

			void _registerSignals();

		public:
			/**
			 * Create a new panel. Dimensionless.
			 */
			panelWidget();

			/**
			 * Create a new panelWidget.
			 * @param pos The panelWidget position.
			 * @param dimension The panelWidget dimension.
			 */
			panelWidget(const vector2& pos, const vector2& dimension);

			/**
			 * Panel destructor. Keep in mind that destroying
			 * the panelWidget will also destroy every child of it, freeing
			 * their memory pointers.
			 */
			virtual ~panelWidget();

			/**
			 * Draw the panel (called internally by guiManager)
			 */
			void draw();

			// handle mousemove
			bool mousemove(signalInfo_t info, void* userData);
			bool mousein(signalInfo_t info, void* userData);
			bool mouseout(signalInfo_t info, void* userData);
	};
	
	/**
	 * \brief Button skin edges
	 */
	extern const char* DLL_EXPORT buttonWidgetEdges[];	

	/**
	 * \brief Button Highlight skin edges
	 */
	extern const char* DLL_EXPORT buttonHighlightWidgetEdges[];

	/**
	 * \brief Button Pushed skin edges
	 */
	extern const char* DLL_EXPORT buttonPushedWidgetEdges[];

	/**
	 * \brief Button Skin.
	 */
	class buttonSkin : public widgetSkin
	{
		protected:
			rectangle* mSkin[3];

		public:
			buttonSkin() : widgetSkin() {}

			void setSkinData(const char** skinArray, rectangle* parentRect);
			void drawWidgetSkin(const vector2& pos);
	};

	/**
	 * \brief Button widget.
	 */
	class DLL_EXPORT buttonWidget : public buttonSkin, public widget, public signalHandler
	{
		protected:
			baseText* mText;
			buttonSkin mHighlightSkin;
			buttonSkin mPushedSkin;

			stateType_t mState;
			void _registerSignals();

			// if mouse was down here, and is up here, emit clicked
			unsigned int mMouseWasDown;

		public:
			/**
			 * Create a new button. Dimensionless.
			 */
			buttonWidget();

			/**
			 * Create a new button.
			 * @param pos The button position.
			 * @param dimension The button dimension.
			 */
			buttonWidget(const vector2& pos, const vector2& dimension);

			/**
			 * Set button text.
			 */
			void setText(const std::string& text);

			/**
			 * Return button text.
			 */
			const std::string& getText() const;

			/**
			 * Resize the button widget, setting skin.
			 */
			void setDimension(const vector2& dimension);

			/**
			 * Button destructor. Destroying the button will destroy
			 * allocated texts inside it too.
			 */
			virtual ~buttonWidget();

			/**
			 * Draw the button (called internally by guiManager)
			 */
			void draw();

			// handle mousemove
			bool mousemove(signalInfo_t info, void* userData);
			bool mousein(signalInfo_t info, void* userData);
			bool mouseout(signalInfo_t info, void* userData);
			bool mousedown(signalInfo_t info, void* userData);
			bool mouseup(signalInfo_t info, void* userData);
	};

	/**
	 * \brief Handle basic GUI operations.
	 */
	class DLL_EXPORT guiManager : public singleton<guiManager>, public inputEventHandler
	{
		protected:
			sticker* mCursor;

			/**
			 * Last cursor position.
			 */
			vector2 mCursorLastPos;

			/**
			 * Cursor Delta position.
			 */
			vector2 mCursorDeltaPos;

			/**
			 * GUI elements skin material.
			 */
			material* mGuiSkin;

			/**
			 * Default GUI font material and *.dat
			 */
			std::pair<std::string, std::string> mFont;

			/**
			 * GUI material dimensions.
			 */
			vector2 mSkinDimensions;

			/**
			 * Skin definitions for panels
			 */
			std::map<std::string, rectangle*> mSkinDefinitions;

			/**
			 * All registered widgets.
			 */
			std::vector<widget*> mWidgets;

			/**
			 * The last element we got button down, in case
			 * we see button up in the same element, generate
			 * the "clicked" signal.
			 */
			unsigned int mLastButton;

		public:
			/**
			 * Controls the GUI System
			 * Keep in mind that this class is automatically 
			 * instantiated by the root
			 */
			guiManager();

			/**
			 * Destructor
			 */
			~guiManager();

			/**
			 * Get skin definition for a named widget type.
			 * @param wname The widget type name.
			 */
			rectangle* getSkinDefinition(const std::string& wname) const;

			/**
			 * Get skin material.
			 */
			material* getSkinMaterial() const
			{
				return mGuiSkin;
			}

			/**
			 * Get skin dimensions.
			 */
			const vector2& getSkinDimensions() const
			{
				return mSkinDimensions;
			}

			/**
			 * Return skin default font material.
			 */
			const std::string& getFontMaterial() const
			{
				return mFont.first;
			}

			/**
			 * Return skin default dat file.
			 */
			const std::string& getFontDatFile() const
			{
				return mFont.second;
			}

			/**
			 * Return this guiManager singleton instance.
			 */
			static guiManager& getSingleton();

			/**
			 * Set gui elements skin material and definitions file.
			 * @param matName The name of the skin material.
			 * @param guiFile The full path to the skin definitions file.
			 */
			void setSkin(const std::string& matName, const std::string& guiFile);

			/**
			 * Set the cursor to show on screen on mouse events
			 * keep in mind that this class needs an renderer
			 * to work automatically. If you dont want to use a renderer
			 * get the cursor sticker and draw it by yourself in your render loop.
			 */
			void setCursor(const std::string& mat, const vector2& scale);

			/**
			 * Push a widget into the widgets list of guiManager.
			 */
			void pushWidget(widget* w);

			/**
			 * Returns the cursor position.
			 */
			const vector2 getCursorPosition() const;

			/**
			 * Returns the cursor movement since last frame.
			 */
			const vector2& getCursorDeltaPosition() const;

			/**
			 * Handle mouse down. Should only
			 * be called if you want to force a button down on the GUI.
			 */
			bool mouseButtonDown(unsigned int button, inputPeripheral* mouse);
			
			/**
			 * Handle mouse up. Should only
			 * be called if you want to force a button up on the GUI.
			 */
			bool mouseButtonUp(unsigned int button, inputPeripheral* mouse);

			/**
			 * Take care of events, handle elements and update cursor position.
			 */
			void update();
	};
}

#endif

