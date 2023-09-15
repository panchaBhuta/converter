## class converter::t_S2Tconv< T_C >

```c++
template< typename T_C >
struct t_S2Tconv;
```

If a 'type-C' satisfies concept 'c_S2Tconverter', then use that 'type-C'; else assume it's a 'type-T' and bumped up using 'ConvertFromStr< type-T >' to create a class staisfying concept 'c_S2Tconverter'. This mechanism enables 'template-converter-algorithm' to handle both 'type-C' and 'type-T' using the same code base, (i.e. reduces code duplicity of Getters and Setters functions).  


**Template Parameters**
 - `T_C`                   T can be data-type such as int, double etc; xOR C -> Conversion class statisfying concept 'c_S2Tconverter'.

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

