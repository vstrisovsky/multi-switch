# multi-switch

c++ multi-switch library

```
_switch<int, std::string>(0, "example")
    <= _case(0, "test")([]{/* do something*/})
    <= _case(1, _)([]{/* do something*/})
    <= _case(_, _)([]{/* do something*/});
```
