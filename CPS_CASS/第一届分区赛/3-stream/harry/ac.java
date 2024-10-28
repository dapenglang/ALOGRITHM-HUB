import java.util.*;
import java.util.function.BinaryOperator;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.stream.Collectors;
import java.util.stream.Stream;


class MyStream<T> {

    // you should NEVER change the signature of any public functions

    public MyStream(List<T> data) {
        // TODO: initialize your stream
        this.data = data;
    }

    public MyStream(Stream<T> stream) {
        this.s = stream;
        fromStream = true;
    }

    public <U> MyStream<U> map(Function<T, U> transformer) {
        // TODO: transform every elements in stream with given transformer
        data = getStream().collect(Collectors.toList());
        fromStream = false;
        return new MyStream<>(getStream().map(transformer));
    }

    public MyStream<T> filter(Predicate<T> condition) {
        // TODO: filter the elements in stream with given condition
        data = getStream().collect(Collectors.toList());
        fromStream = false;
        return new MyStream<>(getStream().filter(condition));
    }

    public <K extends Comparable<K>, V> List<Pair<K, MyStream<V>>> groupByKey(Function<T, K> keyGetter, Function<T, V> valueGetter) {
        // TODO: group the elements by their key (by keyGetter), then initialize one stream for values with same key

        List<Pair<K, MyStream<V>>> result = new ArrayList<>();

        // duplicate the stream
        data = getStream().collect(Collectors.toList());
        fromStream = false;

        data.stream()
                .collect(Collectors.groupingBy(keyGetter,
                        HashMap::new, Collectors.mapping(valueGetter, Collectors.toList())))
                .forEach((k, v) -> result.add(new Pair<>(k, new MyStream<>(v))));

        return result;
    }

    public T reduce(T initial, BinaryOperator<T> reducer) {
        // TODO: reduce the whole stream to one element
        data = getStream().collect(Collectors.toList());
        fromStream = false;
        return getStream().reduce(initial, reducer);
    }

    public List<T> collect() {
        // TODO: return all elements in the stream as an array
        if (fromStream) return getStream().collect(Collectors.toList());
        else return data;
    }

    // you can add or delete anything you want as private members
    private List<T> data;
    private Stream<T> s;
    private boolean fromStream = false;

    private Stream<T> getStream() {
        if (fromStream) return s;
        return data.stream();
    }

}
