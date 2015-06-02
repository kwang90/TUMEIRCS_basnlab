/**
 *\file genericParamInterface.h
 *\brief A generic interface to exchange parameters with a class derived from it.
 *
 *\copyright (c) 2011,2014  Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 * This class is for debugging purposes, only. It is not part of the interface. In Java, the class \ref he2mtSDI won't derive it.
 * In c++, you can safely ignore it as a dummy implementation comes with the code allready.
 *
 * The class is used as a generic interface between classes to exchange certain parameters in a standardized, convenient way.
 * This enables for example the automatic creation of GUIs modifying these parameters.
 *
 * Classes that derive from this class must implement its functions and have to take care to comply with the specifications below.
 * If the specifications are met, params can be exchanged with the class implementing this interface with any other class - without the exchange partner having to know
 * anything of your class that implements GenericParamInterface, except the fact that GenericParamInterface is a base class.
 *
 * -Each string used for param exchange can have max. 255 characters
 * -Each param is a key-value-pair. The ident of a param gives it a name and is an arbitrary string.
 * -The value of the param is a string, containing the value of the param.
 *  -For decimal and floating point values, the format is the string representation of the number (for example, valid value would be: "10", "-1", "0.34", "-0.24")
 *  -For string values, any string can be transported
 * -getParam will fill a string buffer that has to have at least 256 bytes with the current value
 * -setParam will set the param
 * -getParamList will return a list of all params available for that class having the
 *   format specified. It will allocate the buffer needed for that, and the application has to deAlloc the buffer by calling free()
*  	 param List format (EBNF): Ascii file that is fomatted like that:
*   - paramList = {paramLine}
*   - paramLine = ident "|" decident "|" limitMin "|" limitMax "\n"
*   	- ident		: string identifiing the param, must be compatible with the first param of getParam();
*    	- decident	: decident. The first character identifies the value type of the param. Possible values:
*    						"f": float<br>
*    						"d": decimal<br>
*    						"s": string<br>
*    						in case of floats, "f" ma be follwoed by the number of decimals. Example: "f11" => 11 decimals precision<br>
*		- limitMin	 :floating point number specifyng the minimal value of the param. ignored for strings
*		- limitMax	 :floating point number specifying the maximal value of the param. For strings, if 0, the string can have abitrary lenght. Other wise, the max lenght will be set to this value
*
**/

#ifndef GENERIC_PARAM_INTERFACE_H_
#define GENERIC_PARAM_INTERFACE_H_

#include <inttypes.h>
#include <stddef.h>
/**
 * \class genericParamInterface
 * \brief Class that allows the exchange of parameter values in a generic way
 */
class genericParamInterface{
public:

	/**
	 * \brief Read out a param.
	 * Read out a param.
	 *  @param ident Paremter identification string. Memory management is to be done by the caller.
	 *  @param value A writable, empty buffer to be filled by the called function. Memory management is to be done by the caller. At least 256 bytes of buffer need to be provided
	 *  @return true if successful, false, if not
	 */
	virtual bool getParam(const char* ident, char* value){return false;};

	/**
	 *\brief  Set param "ident" to value "value"
	 *
	 *  @param ident Paremter identification string. Memory management is to be done by the caller.
	 *  @param value A buffer containing the value to be set. Memory management is to be done by the caller.
	 * @return
	 */
	virtual bool setParam(const char* ident, const char* value){return false;};

	/**
	 * \brief Generates a machine-readable list of all parameters available
	 *  param List format (EBNF): Ascii string that is fomratted like follows.
	 *    paramList = {paramLine}<br>
	 *    paramLine = ident "|" decident "|" limitMin "|" limitMax "\n"<br>
	 *    ident		: string identifiing the param, must be compatible with the first param of getParam();<br>
	 *    decident	: decident. The first character identifies the value type of the param. Possible values:<br>
	 *    						"f": float<br>
	 *    						"d": decimal<br>
	 *    						"s": string<br>
	 *    			  in case of floats, "f" ma be follwoed by the number of decimals. Example: "f11" => 11 decimals precision<br>
	 *	limitMin	 :floating point number specifyng the minimal value of the param. ignored for strings<br>
	 *	limitMax	 :floating point number specifying the maximal value of the param. For strings, if 0, the string can have abitrary lenght. Other wise, the max lenght will be set to this value<br>
	 *
	 * @return String containing the parameter list. It will be alloc'ed by the function using malloc() and should be free'd after use using free()
	 */
	virtual char* getParamList(){return NULL;};

	/**
	 * \brief Provides a string containing an identifier (i.e. Name) of the class that implements the genericParamInterface
	 * @param ident Non-empty, writable buffer of at least 256 bytes of space to write the name into.
	 */
	virtual void getIdent(char* ident){};

private:
	/**
	 * \brief ascii-to-float-conversion
	 * Not part of the interface as a standard implementation is provided.
	 * @param str string to convert a float
	 * @return float containing the string
	 */
	double atof2(const char* str);
};

#endif /* GENERIC_PARAM_INTERFACE_H_ */
