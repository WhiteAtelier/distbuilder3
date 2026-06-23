# condition について

```json
"condition": true
```

とか

```json
"condition": false
```

とか

object で式を定義できる

```json
"condition": {
    "op": "opname",
    "value": "value"
}
```

op フィールドは必須、あとは opname ごとにほかのフィールドが変化します。

| opname             | 意味                                | 型       |
| ------------------ | ----------------------------------- | -------- |
| `is-true`          | `value` が `true`                   | `bool`   |
| `is-false`         | `value` のすべての要素が `false`    | `bool`   |
| `greater`          | `lhs` よりも `rhs` が大きい         | `double` |
| `less`             | `lhs` よりも `rhs` が小さい         | `double` |
| `greater-or-equal` | `lhs` よりも `rhs` が大きいか等しい | `double` |
| `less-or-equal`    | `lhs` よりも `rhs` が小さいか等しい | `double` |
| `equal`            | `lhs` と `rhs` が等しい             | any      |
| `or`               | いずれかの `values` が `true`       | `bool`   |
| `and`              | すべての `values` が `true`         | `bool`   |

そして、それぞれの値部分には、上の表の「型」に相当するもののほかに、 OptionValue のみの文字列を指定できる。かならず `${` から始まって、 `}` で終わる。文字列埋め込みの時に使えたプリプロセッサは、ここでは使えない。ただ OptionValue を指定するだけの文字列である。

また、object によってネストできる。その object は全体として bool 値として評価され、 bool 値として評価できる。

```json
"condition": {
    "op": "or",
    "values": [
        {
            "op": "is-true",
            "value": "${option.HogeHoge}" // option.HogeHoge を bool 評価する
        },
        {
            "op": "greater",
            "lhs": 10,  // いったん OptionValue に格納し、 OptionValue 同士の比較を行う
            "rhs": "${option.Foobar}"
        },
        {
            "op": "is-false",
            "value": {
                // このブロックはかならず true 評価される.
                "op": "is-true",
                "value": true
            }
        }
    ]
}
```
