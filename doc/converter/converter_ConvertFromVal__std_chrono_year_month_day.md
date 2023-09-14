## class converter::ConvertFromVal< std::chrono::year_month_day, T2S_FORMAT_YMD >

```c++
template<c_formatYMDoss T2S_FORMAT_YMD>
struct ConvertFromVal< std::chrono::year_month_day, T2S_FORMAT_YMD>
```

Specialized implementation handling 'year_month_day' to string conversion.  

**Template Parameters**
 - `T2S_FORMAT_YMD`        Class which encapsulates conversion parameters/directives such as 'Locale' specific for 'year_month_day'.

---

```c++
static std::string ToStr (const std::chrono::year_month_day & val)
```
Converts variable holding 'year_month_day' value to string. The string has the format '%F' -> '%Y-%m-%d'. 

**Parameters**
- `val` input 'year_month_day'. 

**Returns:**
- string. 

---

```c++
static std::string ToStr_args (const std::chrono::year_month_day & val,
                               const std::string::value_type * fmt,
                               [[maybe_unused]] std::string * abbrev = nullptr,
                               [[maybe_unused]] std::chrono::seconds * offset_sec = nullptr)
```
Converts variable holding 'year_month_day' value to string. The string has the format '%F' -> '%Y-%m-%d'. 

**Parameters**
- `val` input 'year_month_day'. 
- `fmt` a format string specifying the output string format. 
- `abbrev` if not null, pointer to an object that will hold the time zone abbreviation or name corresponding to the Z specifier 
- `offset_sec` if not null, pointer to an object that will hold the offset from UTC corresponding to the z specifier 

**Returns:**
- string. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

