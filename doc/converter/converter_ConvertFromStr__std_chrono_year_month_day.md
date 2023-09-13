## class converter::ConvertFromStr< std::chrono::year_month_day, S2T_FORMAT_YMD >

```c++
template<c_formatYMDiss S2T_FORMAT_YMD>
struct ConvertFromStr< std::chrono::year_month_day, S2T_FORMAT_YMD>
```

Specialized implementation handling string to 'year_month_day' conversion.  

**Template Parameters**
- `S2T_FORMAT_YMD`     type satisfying concept 'c_formatYMDiss'.

---

```c++
static return_type ToVal (const std::string & str)
```

Converts string holding 'year_month_day' value. The string has the format '%F' -> '%Y-%m-%d'. 

**Parameters**
- `str` input string representing date. 

**Returns:**
- 'year_month_day'. 

---

```c++
static return_type ToVal_args (const std::string & str,
                               const std::string::value_type * fmt,
                               [[maybe_unused]]std::string * abbrev = nullptr,
                               [[maybe_unused]]std::chrono::minutes * offset = nullptr)
```
Converts string holding 'year_month_day' value. The string has the format '%F' -> '%Y-%m-%d'. 

**Parameters**
- `str` input string representing date. 
- `fmt` a format string of date-string. 
- `abbrev` if not null, pointer to an object that will hold the time zone abbreviation or name corresponding to the Z specifier 
- `offset` if not null, pointer to an object that will hold the offset from UTC corresponding to the z specifier 

**Returns:**
- 'year_month_day'. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

