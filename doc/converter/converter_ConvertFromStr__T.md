## class converter::ConvertFromStr< T, S2T_FORMAT_STREAM >

```c++
template<c_NOT_string T, c_formatISS S2T_FORMAT_STREAM>
struct ConvertFromStr<T, S2T_FORMAT_STREAM>
```

Convertor class implementation for any (non-string)type's, FROM input-string; using 'std::istringstream'.  

**Template Parameters**
- `T                     'type' converted to, from string data. (Not Applicable for 'string to string' conversion.)
- `S2T_FORMAT_STREAM     Class which encapsulates conversion parameters/directives such as using 'Locale'.
---

```c++
static return_type ToVal (const std::string & str)
```
Converts string holding a possibly-numerical value to numerical datatype representation. 

**Parameters**
- `str` input string. 

**Returns:**
- type value if conversion succeeds. This type is any type that's supported by 'std::istringstream'. Else throws 'std::invalid_argument' on conversion failure. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

