#ifndef __VARIABLE_H
#define __VARIABLE_H

namespace Neo
{

/**
 * @brief Describes which type of data is stored in an Variable object.
 */
enum VARIABLE_TYPE
{
	/// A pointer to 0x0
	VARIABLE_NULL = 0,
	/// A boolean variable
	VARIABLE_BOOL,
	/// An integer variable
	VARIABLE_INT,
	/// An unsigned integer variable
	VARIABLE_UINT,
	/// A float variable
	VARIABLE_FLOAT,
	/// A string variable (either const char* or Neo::String)
	VARIABLE_STRING,
	/// A Vector2 variable
	VARIABLE_VEC2,
	/// A Vector3 variable
	VARIABLE_VEC3,
	/// A Vector4 variable
	VARIABLE_VEC4,
	/// A Neo::Quaternion object
	VARIABLE_QUAT,
	/// A TextureRef object
	VARIABLE_TEXTURE_REF
};

/**
 * @brief This templacte class is used to provide access to custom data
 * without messing with void*'s.
 *
 * @author Yannick Pflanzer
 */
template <class T> class Variable
{
private:
	T m_data;
	VARIABLE_TYPE m_type;

public:
	/**
	 * @brief Constructs a new object.
	 *
	 * The data in the given variable will be copied to an
	 * internal buffer. If it is an address, only the address willa be saved.
	 *
	 * Attention: Don't free memory that will be needed!
	 *
	 * @param data The variable to save.
	 * @param type The type of the variable.
	 */
	Variable(T data, VARIABLE_TYPE type) : m_data(data), m_type(type) {}

	/**
	 * @brief Returns the currently saved data.
	 *
	 * @return The data.
	 */
	T getData() { return m_data; }

	/**
	 * @brief Returns the type of the currently saved data.
	 *
	 * @return The type.
	 */
	VARIABLE_TYPE getType() { return m_type; }

	/**
	 * @brief Changes the value of the currently saved data.
	 *
	 * @param data The new value.
	 */
	void setData(T data) { m_data = data; }
};
}
#endif
