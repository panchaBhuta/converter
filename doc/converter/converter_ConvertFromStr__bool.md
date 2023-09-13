## class converter::ConvertFromStr< bool, S2T_Format_WorkAround< bool, PROCESS_ERR > >

```c++
template<FailureS2Tprocess PROCESS_ERR>
struct ConvertFromStr<bool, S2T_Format_WorkAround<bool, PROCESS_ERR> >
```

Specialized implementation handling string to bool conversion.  

**Template Parameters**
- `PROCESS_ERR`        enum variable of type FailureS2Tprocess.


---

```c++
static return_type ToVal (const std::string & str)
```
Converts string holding bool value. 

**Parameters**
- `str` input string. 

**Returns:**
- bool. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

