## class converter::ConvertFromVal< T, T2S_Format_WorkAround >

```c++
template<c_char CH>
struct ConvertFromVal<CH, T2S_Format_WorkAround>
```

Specialized implementation handling char to string conversion.  

**Template Parameters**
- `CH`                    'char-type' converted from, to string data.
---

```c++
static std::string ToStr (const CH & val)
```
Converts char value to string. 

**Parameters**
- `val` input string. 

**Returns:**
- string. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

