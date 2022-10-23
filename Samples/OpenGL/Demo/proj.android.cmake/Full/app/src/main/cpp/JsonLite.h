/**
	
	Copyright(C) 2021, douya.tech, all rights reserved.

	Author: 3h6a@163.com

*/

#ifndef JSONLITE_H
#define JSONLITE_H

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include <map>

#ifndef WIN32
typedef unsigned char BYTE;
typedef unsigned int UINT;
#endif

//----------------------------------------------------------------------------------------------------------------------
// JString

class JString
{
public:
	JString(void) : m_pBuffer(NULL)
	{
	}

	JString(UINT Length) : m_pBuffer(NULL)
	{
		Allocate(Length);
	}

	JString(char pChar, UINT Length) : m_pBuffer(NULL)
	{
		if (Allocate(Length))
		{
			memset(m_pBuffer, pChar, Length);
		}
	}

	JString(const char *pString) : m_pBuffer(NULL)
	{
		if (pString != NULL && Allocate(strlen(pString)))
		{
			strcpy(m_pBuffer, pString);
		}
	}

	JString(const char *pString, UINT Length) : m_pBuffer(NULL)
	{
		if (Allocate(Length) && pString != NULL)
		{
			strncpy(m_pBuffer, pString, Length);
		}
	}

	JString(const JString &Instance) : m_pBuffer(Instance.m_pBuffer)
	{
		AddRef();
	}

	JString &operator=(const JString &Instance)
	{
		if (this != &Instance)
		{
			Release();
			m_pBuffer = Instance.m_pBuffer;
			AddRef();
		}
		return *this;
	}

	~JString(void)
	{
		Release();
	}

private:
	void Copy()
	{
		if (m_pBuffer != NULL)
		{
			//old refcnt
			UINT &refcnt = *(UINT *)(m_pBuffer - sizeof(UINT));
			if (refcnt > 1)
			{
				//old string
				char *pString = GetString();
				if (Allocate(GetLength()))
				{
					strcpy(m_pBuffer, pString);
				}
				//old refcnt
				--refcnt;
			}
		}
	}

	// 0 1 2 3 | 4 5 6 7 | ... | n
	// length    refcnt          0
	bool Allocate(UINT Length)
	{
		m_pBuffer = new (std::nothrow) char[sizeof(UINT) + sizeof(UINT) + Length + 1];

		if (m_pBuffer != NULL)
		{
			UINT *p = (UINT *)m_pBuffer;
			*p = Length;
			++p;
			*p = 1;
			++p;
			m_pBuffer = (char *)p;
			memset(m_pBuffer, 0, Length + 1);
		}

		return m_pBuffer != NULL;
	}

	void AddRef()
	{
		if (m_pBuffer != NULL)
		{
			UINT *p = (UINT *)m_pBuffer;
			--p;
			++(*p);
		}
	}

	void Release()
	{
		if (m_pBuffer != NULL)
		{
			UINT *p = (UINT *)m_pBuffer;
			--p;
			if (--(*p) == 0)
			{
				delete[]-- p;
			}
			m_pBuffer = NULL;
		}
	}

public:
	operator char *() const
	{
		return GetString();
	}

	bool operator==(const char *pString) const
	{
		return Equals(pString);
	}

	bool operator!=(const char *pString) const
	{
		return !Equals(pString);
	}

	UINT Capacity() const
	{
		UINT result = 0;
		if (m_pBuffer != NULL)
		{
			UINT *p = (UINT *)m_pBuffer;
			--p;
			--p;
			result = *p;
		}
		return result;
	}

	int GetLength() const
	{
		int result = 0;
		if (m_pBuffer != NULL)
		{
			result = strlen(m_pBuffer);
		}
		return result;
	}

	BYTE *GetBuffer() const
	{
		return (BYTE *)m_pBuffer;
	}

	char *GetString() const
	{
		return m_pBuffer;
	}

	bool Equals(const char *pString) const
	{
		bool result = false;
		if (m_pBuffer != NULL && pString != NULL)
		{
			result = strcmp(m_pBuffer, pString) == 0;
		}
		else if (pString != NULL && *pString == 0) //m_pBuffer == NULL
		{
			result = true;
		}
		else if (m_pBuffer != NULL && *m_pBuffer == 0) //pString == NULL
		{
			result = true;
		}
		else //m_pBuffer == NULL && pString == NULL
		{
			result = true;
		}
		return result;
	}

	bool IsEmpty() const
	{
		return m_pBuffer == NULL || *m_pBuffer == 0;
	}

	void Empty()
	{
		*this = JString();
	}

	JString &operator+=(char chr)
	{
		Copy();

		UINT length = GetLength();
		if (length >= Capacity())
		{
			*this = JString(m_pBuffer, length + 16); //Step = 16
		}
		m_pBuffer[length] = chr;

		return *this;
	}

	JString &operator+=(const char *pString)
	{
		if (pString != NULL && *pString != 0)
		{
			Copy();

			UINT length = GetLength() + strlen(pString);
			if (length > Capacity())
			{
				*this = JString(m_pBuffer, length + 256); //Step = 256
			}
			strcat(m_pBuffer, pString);
		}

		return *this;
	}

	JString &AppendFormat(const char *Format, ...)
	{
		Copy();

		va_list argptr;

		va_start(argptr, Format);
		UINT length = GetLength() + vsnprintf(NULL, 0, Format, argptr);
		va_end(argptr);

		if (length > Capacity())
		{
			*this = JString(m_pBuffer, length);
		}

		va_start(argptr, Format);
		vsprintf(m_pBuffer + GetLength(), Format, argptr);
		va_end(argptr);

		return *this;
	}

	JString &Format(const char *Format, ...)
	{
		Copy();

		va_list argptr;

		va_start(argptr, Format);
		UINT length = vsnprintf(NULL, 0, Format, argptr);
		va_end(argptr);

		if (length > Capacity())
		{
			*this = JString(length);
		}

		va_start(argptr, Format);
		vsprintf(m_pBuffer, Format, argptr);
		va_end(argptr);

		return *this;
	}

	JString &Trim()
	{
		if (!IsEmpty())
		{
			Copy();

			//trim tail
			char *p = m_pBuffer + GetLength();
			while (isspace(*--p) && p >= m_pBuffer)
			{
				*p = 0;
			}

			//trim head
			char *b = m_pBuffer;
			while (isspace(*b))
			{
				++b;
			}

			//move data
			if (b != m_pBuffer)
			{
				memmove(m_pBuffer, b, GetLength());
			}
		}

		return *this;
	}

	JString &Trim(const char *pString)
	{
		if (!IsEmpty() && pString != NULL && *pString != 0)
		{
			Copy();

			//trim tail
			char *p = m_pBuffer + GetLength();
			while (strchr(pString, *--p) != NULL && p >= m_pBuffer)
			{
				*p = 0;
			}

			//trim head
			char *b = m_pBuffer;
			while (strchr(pString, *b) != NULL)
			{
				++b;
			}

			//move data
			if (b != m_pBuffer)
			{
				memmove(m_pBuffer, b, GetLength());
			}
		}

		return *this;
	}

private:
	char *m_pBuffer;
};

inline bool operator<(const JString &String1, const JString &String2)
{
	return strcmp(String1, String2) < 0;
}

inline JString operator+(const JString &String1, const JString &String2)
{
	JString result(strlen(String1) + strlen(String2));
	sprintf(result.GetString(), "%s%s", String1.GetString(), String2.GetString());
	return result;
}

//----------------------------------------------------------------------------------------------------------------------
// JPointer

template <class T>
class JPointer
{
public:
	JPointer() : m_pCounter(NULL), m_pPointer(NULL)
	{
	}

	JPointer(T *pPointer) : m_pCounter(NULL), m_pPointer(pPointer)
	{
		m_pCounter = new (std::nothrow) UINT(1);
	}

	JPointer(const JPointer &Instance) : m_pCounter(Instance.m_pCounter), m_pPointer(Instance.m_pPointer)
	{
		AddRef();
	}

	JPointer &operator=(const JPointer &Instance)
	{
		if (this != &Instance)
		{
			Release();
			m_pCounter = Instance.m_pCounter;
			m_pPointer = Instance.m_pPointer;
			AddRef();
		}
		return *this;
	}

	~JPointer()
	{
		Release();
	}

	void AddRef()
	{
		if (m_pCounter != NULL)
		{
			++*m_pCounter;
		}
	}

	void Release()
	{
		if (m_pCounter != NULL)
		{
			if (--*m_pCounter == 0)
			{
				delete m_pPointer;
				delete m_pCounter;
			}
			m_pCounter = NULL;
		}
	}

public:
	T *operator->() const
	{
		return m_pPointer;
	}

	T *GetPointer() const
	{
		return m_pPointer;
	}

	template <typename N>
	N *GetPointer() const
	{
		return dynamic_cast<N *>(m_pPointer);
	}

private:
	UINT *m_pCounter;
	T *m_pPointer;
};

//----------------------------------------------------------------------------------------------------------------------
// IJsonValue

class IJsonValue
{
public:
	virtual ~IJsonValue() {}

public:
	virtual JString ToString() const = 0;
	virtual JString GetType() const = 0;
	virtual JString GetString() const = 0;
	virtual int GetInteger() const = 0;
	virtual bool GetBoolean() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
// JsonNull

class JsonNull : public IJsonValue
{
public:
	JsonNull()
	{
	}

	virtual JString ToString() const
	{
		return GetString();
	}

	virtual JString GetType() const
	{
		return JString("null");
	}

	virtual JString GetString() const
	{
		return JString("null");
	}

	virtual int GetInteger() const
	{
		return 0;
	}

	virtual bool GetBoolean() const
	{
		return false;
	}
};

//----------------------------------------------------------------------------------------------------------------------
// JsonBoolean

class JsonBoolean : public IJsonValue
{
public:
	JsonBoolean(bool Value) : m_Value(Value)
	{
	}

	virtual JString ToString() const
	{
		return GetString();
	}

	virtual JString GetType() const
	{
		return JString("boolean");
	}

	virtual JString GetString() const
	{
		if (m_Value)
		{
			return JString("true");
		}
		else
		{
			return JString("false");
		}
	}

	virtual int GetInteger() const
	{
		if (m_Value)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	virtual bool GetBoolean() const
	{
		return m_Value;
	}

private:
	bool m_Value;
};

//----------------------------------------------------------------------------------------------------------------------
// JsonNumber

class JsonNumber : public IJsonValue
{
public:
	JsonNumber(const char *Value) : m_Value(Value)
	{
	}

	JsonNumber(int Value)
	{
		m_Value.Format("%d", Value);
	}

	JsonNumber(double Value)
	{
		m_Value.Format("%g", Value);
	}

	virtual JString ToString() const
	{
		return GetString();
	}

	virtual JString GetType() const
	{
		return JString("number");
	}

	virtual JString GetString() const
	{
		return m_Value;
	}

	virtual int GetInteger() const
	{
		return atol(m_Value);
	}

	virtual bool GetBoolean() const
	{
		return GetInteger() != 0;
	}

	double GeDouble() const
	{
		return atof(m_Value);
	}

private:
	JString m_Value;
};

//----------------------------------------------------------------------------------------------------------------------
// JsonString

class JsonString : public IJsonValue
{
public:
	JsonString(const char *pString) : m_Value(pString)
	{
	}

	virtual JString ToString() const
	{
		return JString().Format("\"%s\"", m_Value.GetString());
	}

	virtual JString GetType() const
	{
		return JString("string");
	}

	virtual JString GetString() const
	{
		return m_Value;
	}

	virtual int GetInteger() const
	{
		return atol(m_Value);
	}

	virtual bool GetBoolean() const
	{
		return GetInteger() != 0;
	}

private:
	JString m_Value;
};

//----------------------------------------------------------------------------------------------------------------------
// JsonArray

class JsonArray : public IJsonValue
{
public:
	JsonArray()
	{
	}

	virtual JString ToString() const
	{
		JString result("[", 256);
		for (UINT i = 0, l = m_Items.size(); i < l;)
		{
			result += m_Items[i]->ToString();
			if (++i < l)
			{
				result += ", ";
			}
		}
		result += "]";
		return result;
	}

	virtual JString GetType() const
	{
		return JString("array");
	}

	virtual JString GetString() const
	{
		return JString("array");
	}

	virtual int GetInteger() const
	{
		return m_Items.size();
	}

	virtual bool GetBoolean() const
	{
		return GetInteger() > 0;
	}

public:
	JsonArray Add(JPointer<IJsonValue> pValue)
	{
		m_Items.push_back(pValue);
		return *this;
	}

	JPointer<IJsonValue> Get(UINT Index)
	{
		if (Index < m_Items.size())
		{
			return m_Items[Index];
		}
		return new JsonNull();
	}

protected:
	std::vector<JPointer<IJsonValue>> m_Items;
};

//----------------------------------------------------------------------------------------------------------------------
// JsonObject

class JsonObject : public IJsonValue
{
public:
	JsonObject()
	{
	}

	virtual JString ToString() const
	{
		JString result("{", 256);
		for (UINT i = 0, l = m_Names.size(); i < l;)
		{
			JString name = m_Names[i];
			JString item = m_Items.find(name)->second->ToString();
			result += JString().Format("\"%s\": %s", name.GetString(), item.GetString());
			if (++i < l)
			{
				result += ", ";
			}
		}
		result += "}";
		return result;
	}

	virtual JString GetType() const
	{
		return JString("object");
	}

	virtual JString GetString() const
	{
		return JString("object");
	}

	virtual int GetInteger() const
	{
		return m_Names.size();
	}

	virtual bool GetBoolean() const
	{
		return GetInteger() > 0;
	}

public:
	JsonObject Set(const char *pName, JPointer<IJsonValue> pValue)
	{
		if (m_Items.find(pName) == m_Items.end())
		{
			m_Names.push_back(pName);
		}
		m_Items[pName] = pValue;
		return *this;
	}

	JString GetName(UINT Index) const
	{
		JString result;
		if (Index < m_Names.size())
		{
			result = m_Names[Index];
		}
		return result;
	}

	bool Contain(const char *pName) const
	{
		return m_Items.find(pName) != m_Items.end();
	}

	JPointer<IJsonValue> Get(const char *pName) const
	{
		std::map<JString, JPointer<IJsonValue>>::const_iterator itr = m_Items.find(pName);
		if (itr != m_Items.end())
		{
			return itr->second;
		}
		return new JsonNull();
	}

protected:
	std::vector<JString> m_Names;
	std::map<JString, JPointer<IJsonValue>> m_Items;
};

//----------------------------------------------------------------------------------------------------------------------
// JsonBuilder

class JsonBuilder
{
public:
	virtual ~JsonBuilder() {}

public:
	virtual operator JPointer<IJsonValue>() = 0;
	virtual JString Build() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
// JsonArrayBuilder

class JsonArrayBuilder : public JsonBuilder
{
public:
	JsonArrayBuilder() : m_pValue(new JsonArray())
	{
	}

public:
	virtual operator JPointer<IJsonValue>()
	{
		return m_pValue;
	}
	virtual JString Build()
	{
		return m_pValue->ToString();
	}

public:
	JsonArrayBuilder Add(JPointer<IJsonValue> pValue)
	{
		m_pValue.GetPointer<JsonArray>()->Add(pValue);
		return *this;
	}
	JsonArrayBuilder Add(const char *pValue)
	{
		return Add(new JsonString(pValue));
	}
	JsonArrayBuilder Add(int Value)
	{
		return Add(new JsonNumber(Value));
	}
	JsonArrayBuilder Add(double pValue)
	{
		return Add(new JsonNumber(pValue));
	}
	JsonArrayBuilder AddBool(bool pValue)
	{
		return Add(new JsonBoolean(pValue));
	}
	JsonArrayBuilder AddNull()
	{
		return Add(new JsonNull());
	}

private:
	JPointer<IJsonValue> m_pValue;
};

//----------------------------------------------------------------------------------------------------------------------
// JsonObjectBuilder

class JsonObjectBuilder : public JsonBuilder
{
public:
	JsonObjectBuilder() : m_pValue(new JsonObject())
	{
	}

public:
	virtual operator JPointer<IJsonValue>()
	{
		return m_pValue;
	}
	virtual JString Build()
	{
		return m_pValue->ToString();
	}

public:
	JsonObjectBuilder Set(const char *pName, JPointer<IJsonValue> pValue)
	{
		m_pValue.GetPointer<JsonObject>()->Set(pName, pValue);
		return *this;
	}
	JsonObjectBuilder Set(const char *pName, const char *pValue)
	{
		return Set(pName, new JsonString(pValue));
	}
	JsonObjectBuilder Set(const char *pName, int Value)
	{
		return Set(pName, new JsonNumber(Value));
	}
	JsonObjectBuilder Set(const char *pName, double pValue)
	{
		return Set(pName, new JsonNumber(pValue));
	}
	JsonObjectBuilder SetBool(const char *pName, bool pValue)
	{
		return Set(pName, new JsonBoolean(pValue));
	}
	JsonObjectBuilder SetNull(const char *pName)
	{
		return Set(pName, new JsonNull());
	}

private:
	JPointer<IJsonValue> m_pValue;
};

//----------------------------------------------------------------------------------------------------------------------
// JsonValue

class JsonValue
{
public:
	JsonValue(JPointer<IJsonValue> pValue) : m_pValue(pValue)
	{
	}

public:
	JPointer<IJsonValue> operator->() const
	{
		return m_pValue;
	}

	operator bool() const
	{
		return m_pValue->GetBoolean();
	}

	UINT GetLength() const
	{
		return m_pValue->GetInteger();
	}

	JString GetName(UINT Index) const
	{
		JsonObject *p = m_pValue.GetPointer<JsonObject>();
		if (p != NULL)
		{
			return p->GetName(Index);
		}
		return JString();
	}

	bool Contain(const char *pName) const
	{
		JsonObject *p = m_pValue.GetPointer<JsonObject>();
		if (p != NULL)
		{
			return p->Contain(pName);
		}
		return false;
	}

	JsonValue Get(const JString &pName) const
	{
		JsonObject *p = m_pValue.GetPointer<JsonObject>();
		if (p != NULL)
		{
			return JsonValue(p->Get(pName));
		}
		return JsonValue(new JsonNull());
	}

	JsonValue Get(UINT Index) const
	{
		JsonArray *pAry = m_pValue.GetPointer<JsonArray>();
		if (pAry != NULL)
		{
			return JsonValue(pAry->Get(Index));
		}
		JsonObject *pObj = m_pValue.GetPointer<JsonObject>();
		if (pObj != NULL)
		{
			return JsonValue(pObj->Get(pObj->GetName(Index)));
		}
		return JsonValue(new JsonNull());
	}

	//JsonArray wrapper

	JsonValue operator[](UINT Index) const
	{
		return Get(Index);
	}

	JString GetString(UINT Index) const
	{
		return Get(Index)->GetString();
	}

	int GetInteger(UINT Index) const
	{
		return Get(Index)->GetInteger();
	}

	double GetFloat(UINT Index) const
	{
		JsonNumber *p = Get(Index).m_pValue.GetPointer<JsonNumber>();
		if (p != NULL)
		{
			return p->GeDouble();
		}
		return 0.0;
	}

	//JsonObject wrapper

	JsonValue operator[](const JString &Name) const
	{
		return Get(Name);
	}

	JString GetString(const JString &pName) const
	{
		return Get(pName)->GetString();
	}

	int GetInteger(const JString &pName) const
	{
		return Get(pName)->GetInteger();
	}

	double GetFloat(const JString &pName) const
	{
		JsonNumber *p = Get(pName).m_pValue.GetPointer<JsonNumber>();
		if (p != NULL)
		{
			return p->GeDouble();
		}
		return 0.0;
	}

private:
	JPointer<IJsonValue> m_pValue;
};

//----------------------------------------------------------------------------------------------------------------------
// JsonParser

class JsonParser
{
public:
	static JsonValue Parse(const char *pString)
	{
		const char *p = pString;

		//trim head spaces
		while (isspace(*p))
		{
			++p;
		}

		if ('{' == *p)
		{
			return JsonValue(JsonParser().ParseObject(&p));
		}
		else if ('[' == *p)
		{
			return JsonValue(JsonParser().ParseArray(&p));
		}
		else
		{
			return JsonValue(new JsonNull());
		}
	}

private:
	IJsonValue *ParseObject(const char **ppText)
	{
		JsonObject *pObject = new JsonObject();

		const char *&p = ++*ppText;
		while (*p != 0)
		{
			JString name;

			//[]"string":value
			if (!SkipSpace(&p))
				break;

			//["]string":value
			if (*p != '\"')
			{
				break;
			}

			//"[string]":value
			name = ParseString(&p);

			//"string["]:value
			if (*p++ != '\"')
			{
				break;
			}

			//"string"[]:value
			if (!SkipSpace(&p))
				break;

			//"string"[:]value
			if (*p++ != ':')
			{
				break;
			}

			//"string":[]value
			if (!SkipSpace(&p))
				break;

			//"string":[value]
			pObject->Set(name, ParseValue(&p));

			//value[]
			SkipSpace(&p);

			if (*p == ',')
			{
				++p;
			}
			else if (*p == '}')
			{
				break;
			}
			else if (*p == ']') //Error
			{
				break;
			}
			else
			{
				//May be '\0' only.
			}
		}

		return pObject;
	}

	IJsonValue *ParseArray(const char **ppText)
	{
		JsonArray *pArray = new JsonArray();

		const char *&p = ++*ppText;
		while (*p != 0)
		{
			//[]value
			if (!SkipSpace(&p))
				break;

			//[value]
			pArray->Add(ParseValue(&p));

			//value[]
			SkipSpace(&p);

			if (*p == ',')
			{
				++p;
			}
			else if (*p == ']')
			{
				break;
			}
			else if (*p == '}') //Error
			{
				break;
			}
			else
			{
				//May be '\0' only.
			}
		}

		return pArray;
	}

	IJsonValue *ParseValue(const char **ppText)
	{
		IJsonValue *result = NULL;

		const char *&p = *ppText;
		if (*p == '\"')
		{
			JString value = ParseString(&p);
			++p; //'\"'
			result = new JsonString(value);
		}
		else if (*p == '[')
		{
			result = ParseArray(&p);
			++p; //']'
		}
		else if (*p == '{')
		{
			result = ParseObject(&p);
			++p; //'}'
		}
		else
		{
			JString value;
			while (*p != ',' && *p != '}' && *p != ']' && *p != 0)
			{
				value += *p++;
			}
			value.Trim();

			if (value.Equals("true"))
			{
				result = new JsonBoolean(true);
			}
			else if (value.Equals("false"))
			{
				result = new JsonBoolean(false);
			}
			else if (value.Equals("null"))
			{
				result = new JsonNull();
			}
			else
			{
				result = new JsonNumber(value);
			}
		}

		return result;
	}

	JString ParseString(const char **ppText)
	{
		JString result;
		const char *&p = ++*ppText;
		while (*p != '\"' && *p != 0)
		{
			if (*p == '\\')
			{
				switch (*++p)
				{
				case '\"':
					result += '\"';
					break;
				case '\\':
					result += '\\';
					break;
					//case '/':
					//	result += '/';
					//	break;
				case 'b':
					result += '\b';
					break;
				case 'f':
					result += '\f';
					break;
				case 'n':
					result += '\n';
					break;
				case 'r':
					result += '\r';
					break;
				case 't':
					result += '\t';
					break;
				case 'u':
					result += '\\';
					result += 'u';
					result += *++p;
					result += *++p;
					result += *++p;
					result += *++p;
					break;
				default:;
				}
				++p;
			}
			else
			{
				result += *p;
				++p;
			}
		}
		return result;
	}

	bool SkipSpace(const char **ppText)
	{
		const char *&p = *ppText;
		while (isspace(*p))
			++p;
		return *p != 0;
	}
};

class JsonFormater
{
public:
	static JString Format(const JsonValue &Json, int Indent)
	{
		JString result;

		JString type = Json->GetType();
		if (type.Equals("object"))
		{
			result = JsonFormater().FormatObject(Json, 0, Indent);
		}
		else if (type.Equals("array"))
		{
			result = JsonFormater().FormatArray(Json, 0, Indent);
		}
		else
		{
			//Error
		}

		return result;
	}

private:
	JString FormatObject(const JsonValue &Json, int Level, int Indent)
	{
		JString result;
		// first line begin
		if (Indent > 0)
		{
			result += JString(' ', Indent * Level);
		}
		result += '{';
		if (Indent > 0)
		{
			result += '\n';
		}
		// first line end
		for (int i = 0, l = Json.GetLength(); i < l;)
		{
			// begin of the line
			result += JString(' ', Indent * (Level + 1));

			//name begin
			result += FormatString(Json.GetName(i));
			result += ':';
			result += ' ';
			//name end

			JString type = Json[i]->GetType();
			if (type.Equals("string"))
			{
				result += FormatString(Json[i]->GetString());
			}
			else if (type.Equals("object"))
			{
				if (Indent > 0)
				{
					result += '\n';
				}
				result += FormatObject(Json[i], Level + 1, Indent);
			}
			else if (type.Equals("array"))
			{
				if (Indent > 0)
				{
					result += '\n';
				}
				result += FormatArray(Json[i], Level + 1, Indent);
			}
			else
			{
				result += Json[i]->GetString();
			}
			if (++i < l) // Last pair
			{
				result += ',';
			}
			// end of the line
			if (Indent > 0)
			{
				result += '\n';
			}
		}
		// last line begin
		if (Indent > 0)
		{
			result += JString(' ', Indent * Level);
		}
		result += '}';
		// last line end
		return result;
	}

	JString FormatArray(const JsonValue &Json, int Level, int Indent)
	{
		JString result;
		// first line begin
		if (Indent > 0)
		{
			result += JString(' ', Indent * Level);
		}
		result += '[';
		if (Indent > 0)
		{
			result += '\n';
		}
		// first line end
		for (int i = 0, l = Json.GetLength(); i < l;)
		{
			JString type = Json[i]->GetType();
			if (type.Equals("string"))
			{
				// begin of the line
				result += JString(' ', Indent * (Level + 1));

				result += FormatString(Json[i]->GetString());
			}
			else if (type.Equals("object"))
			{
				result += FormatObject(Json[i], Level + 1, Indent);
			}
			else if (type.Equals("array"))
			{
				result += FormatArray(Json[i], Level + 1, Indent);
			}
			else
			{
				// begin of the line
				result += JString(' ', Indent * (Level + 1));

				result += Json[i]->GetString();
			}
			if (++i < l) //Last Value
			{
				result += ',';
			}
			// end of the line
			if (Indent > 0)
			{
				result += '\n';
			}
		}
		// last line begin
		if (Indent > 0)
		{
			result += JString(' ', Indent * Level);
		}
		result += ']';
		// last line end
		return result;
	}

	JString FormatString(const char *pString)
	{
		JString result;

		result += '\"';
		if (pString != NULL)
		{
			while (*pString != 0)
			{
				char chr = *pString;
				switch (chr)
				{
				case '\"':
					result += '\\';
					result += '\"';
					break;
				case '\\':
					if (pString[1] != 'u')
					{
						result += '\\';
						result += '\\';
					}
					else
					{
						result += '\\';
						result += *++pString;
						result += *++pString;
						result += *++pString;
						result += *++pString;
						result += *++pString;
					}
					break;
					//case '/':
					//	result += '\\';
					//	result += '/';
					//	break;
				case '\b':
					result += '\\';
					result += 'b';
					break;
				case '\f':
					result += '\\';
					result += 'f';
					break;
				case '\n':
					result += '\\';
					result += 'n';
					break;
				case '\r':
					result += '\\';
					result += 'r';
					break;
				case '\t':
					result += '\\';
					result += 't';
					break;
				default:
					result += chr;
				}
				++pString;
			}
		}
		result += '\"';

		return result;
	}
};

#endif //End of JSONLITE_H
