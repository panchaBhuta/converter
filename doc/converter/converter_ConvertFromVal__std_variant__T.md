## class converter::ConvertFromVal< std::variant< T, std::string >, T2S_FORMAT >

```c++
template<c_NOT_string T, typename T2S_FORMAT >
struct ConvertFromVal<std::variant<T, std::string>, T2S_FORMAT>
```

Convertor class implementation for any types, TO string. Data is 'variant<T, string>' represents either of type `T` or string if underlying data has error.

**Template Parameters**
- `T`                     'type' converted from, to string data. (Not Applicable for string to string conversion)
- `T2S_FORMAT`            Class which encapsulates conversion parameters/directives such as using 'Locale'.

---

```c++
static std::string ToStr (const input_type & val)
```
Converts `variant<T, string>` value to string. If input variant has T then 'T->S' conversion is applied, else variant's underlying string value is returned. 

**Parameters**
- `val` input 'variant<T,string>' value. 

**Returns:**
- string. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

