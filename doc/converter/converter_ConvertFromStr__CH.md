## class converter::ConvertFromStr< CH, S2T_Format_WorkAround< CH, PROCESS_ERR > >

```c++
template<c_char CH, FailureS2Tprocess PROCESS_ERR>
struct ConvertFromStr< CH, S2T_Format_WorkAround<CH, PROCESS_ERR> >
```

Specialized implementation handling string to char conversion.  

**Template Parameters**
- `CH`              'char-type' converted to, from string data.
- `PROCESS_ERR`     enum variable of type FailureS2Tprocess.

---

```c++
static return_type ToVal (const std::string & str)
```
Converts string holding char value. 

**Parameters**
- `str` input string. 

**Returns:**
- char-type. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

