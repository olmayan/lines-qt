#ifndef LISTFUNC_H
#define LISTFUNC_H

template<typename T1, typename T2>
QVector<T2> map(T2 (*func)(T1), QVector<T1> vector)
{
    QVector<T2> res;
    int count = vector.count();
    res.resize(count);
    for (int i = 0; i < count; i++)
        res[i] = func(vector[i]);
    return res;
}

#endif // LISTFUNC_H
