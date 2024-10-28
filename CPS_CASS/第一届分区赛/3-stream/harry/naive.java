import java.util.*;
import java.util.function.BinaryOperator;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.stream.Collectors;


class MyStream<T> {

    // you should NEVER change the signature of any public functions

    public MyStream(List<T> data) {
        // TODO: initialize your stream
        this.data = data;
    }

    public <U> MyStream<U> map(Function<T, U> transformer) {
        // TODO: transform every elements in stream with given transformer
        return new MyStream<>(this.data.stream()
                .map(transformer)
                .collect(Collectors.toList())
        );
    }

    public MyStream<T> filter(Predicate<T> condition) {
        // TODO: filter the elements in stream with given condition
        return new MyStream<>(this.data.stream()
                .filter(condition)
                .collect(Collectors.toList())
        );
    }

    public <K extends Comparable<K>, V> List<Pair<K, MyStream<V>>> groupByKey(Function<T, K> keyGetter, Function<T, V> valueGetter) {
        // TODO: group the elements by their key (by keyGetter), then initialize one stream for values with same key
        HashMap<K, ArrayList<V>> map = new HashMap<>();
        for (T t : data) {
            K key = keyGetter.apply(t);
            V value = valueGetter.apply(t);
            if (map.containsKey(key)) {
                map.get(key).add(value);
            } else {
                ArrayList<V> list = new ArrayList<>();
                list.add(value);
                map.put(key, list);
            }
        }
        ArrayList<Pair<K, MyStream<V>>> result = new ArrayList<>();
        for (Map.Entry<K, ArrayList<V>> en: map.entrySet()) {
            result.add(new Pair<>(en.getKey(), new MyStream<>(en.getValue())));
        }
        return result;
    }

    public T reduce(T initial, BinaryOperator<T> reducer) {
        // TODO: reduce the whole stream to one element
        return this.data.stream().reduce(initial, reducer);
    }

    public List<T> collect() {
        // TODO: return all elements in the stream as an array
        return this.data;
    }

    // you can add or delete anything you want as private members
    private List<T> data;
}

