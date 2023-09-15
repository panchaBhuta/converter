## class converter::t_T2Sconv< T_C >

```c++
template< typename T_C >
struct t_T2Sconv;
```

If a 'type-C' satisfies concept 'c_T2Sconverter', then use that 'type-C'; else assume it's a 'type-T' and bumped up using 'ConvertFromVal< type-T >' to create a class staisfying concept 'c_T2Sconverter'. This mechanism enables 'template-converter-algorithm' to handle both 'type-C' and 'type-T' using the same code base, (i.e. reduces code duplicity).  


**Template Parameters**
 - `T_C`                   T can be data-type such as int, double etc ; xOR  C -> Conversion class statisfying concept 'c_T2Sconverter'.

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

