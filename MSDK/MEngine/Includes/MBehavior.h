/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MBehavior.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#ifndef _M_BEHAVIOR_H
#define _M_BEHAVIOR_H


// class for creating behaviors
/**
 * @brief The MBehavior class is an abstraction for creating behaviors.
 *
 * Behaviors can be attached to objects in a Maratis scene.
 * They contain several methods for interacting with the engine and the parent object allowing them
 * to be used as one of the main interfaces for programming game logic.
 */
class M_ENGINE_EXPORT MBehavior
{
public:

    /**
     * @brief Creates a new behavior as the child of the given object.
     * @param parentObject The parent of the new object.
     */
	MBehavior(MObject3d * parentObject);
	virtual ~MBehavior(void);

private:

	MObject3d * m_parentObject;

public:

	// destroy
	virtual void destroy(void) = 0;

	// parent
    /**
     * @brief Returns the parent object.
     * @return The parent object.
     */
	inline MObject3d * getParentObject(void){ return m_parentObject; }

	// name
    /**
     * @brief Returns the behavior name.
     *
     * All behaviors need a static string with the name in it to identify it.
     * This method has to be overwritten by any child class!
     *
     * @return The name of the behavior.
     */
	virtual const char * getName(void) = 0;

	// get copy
    /**
     * @brief Copies one instance of a behavior to a new one and sets the new parent.
     * @param parentObject The parent object of the new behavior instance.
     * @return The new behavior instance.
     */
	virtual MBehavior * getCopy(MObject3d * parentObject) = 0;

	// events
    /**
     * @brief This method is used to update the game logic which is implemented here.
     *
     * Update is called every 16,6ms (60FPS) and has to be overwritten by any child class.
     *
     * \b ATTENTION: No rendering can be done in here! Use MBehavior::draw instead!
     *
     */
	virtual void update(void) = 0;

    /**
     * @brief Draws behavior specific images to the screen.
     *
     * Draw is called as often as possible to allow maximum rendering performance.
     *
     * \b ATTENTION: No game logics should be located here since this code is very performance relevant!
     */
	virtual void draw(void){}
	virtual void runEvent(int param){}

	// variables
    /**
     * @brief Returns the number of variables this behavior contains.
     * This number is hardcoded in most behaviors.
     *
     * @see MVariable
     *
     * @return Number of variables.
     */
	virtual unsigned int getVariablesNumber(void) = 0;

    /**
     * @brief Returns the variable with the given ID.
     *
     * Most behaviors give hardcoded variables back.
     *
     * Example:
     * \code
     *
     *  MVariable DataBehavior::getVariable(unsigned int id)
     *  {
     *      switch(id)
     *       {
     *       default:
     *           return MVariable("NULL", NULL, M_VARIABLE_NULL);
     *       case 0:
     *           return MVariable("data", &m_data, M_VARIABLE_STRING);
     *       }
     *   }
     *
     * \endcode
     *
     * @see MVariable
     *
     * @param id The ID of the variable. All values are smaller than the one returned by MBehavior::getVariablesNumber
     * @return The variable.
     */
	virtual MVariable getVariable(unsigned int id) = 0;
};


#endif
