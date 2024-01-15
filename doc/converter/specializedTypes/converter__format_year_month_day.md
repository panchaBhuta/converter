## class converter::format_year_month_day< dateFMT,  PROCESS_ERR >

Template Class representing a CSV document.

---

```c++
  template<const char * dateFMT, converter::FailureS2Tprocess PROCESS_ERR>
  template<typename... Args>
  converter::format_year_month_day (Args &&... args);
```
Constructor.

**Template Parameters**
- `...Args`       Type list of constructor arguments.

**Parameters**
- `...args` constructor arguments as required for underlying base class `std::chrono::year_month_day`.

---
