/*
 * Copyright (c) 2020 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstdint>
#include <array>
#include <vector>
#include <tuple>

#include "delta.h"
#include "objects.h"

using namespace std;

namespace reone {

namespace mp {

template <class TuplePtr, size_t ... Is>
auto getSerImpl(TuplePtr &&seqPtr, integer_sequence<size_t, Is...>) {
    return make_tuple(*(get<Is>(seqPtr))...);
}

template <class T>
auto getSerial(T &obj) {
    return getSerImpl(getSerialPointers(obj), make_index_sequence<tuple_size<decltype(getSerialPointers(obj))>::value>());
}

/* helper functions */

using expander = int[]; // https://stackoverflow.com/a/25683817

template <class Tuple, size_t I>
void diffHeaderIter(const Tuple &left, const Tuple &right, uint16_t &header) {
    if (get<I>(left) != get<I>(right)) {
        header |= 1 << I;
    }
}

template <class Tuple, size_t ... Is>
uint16_t diffHeader(const Tuple &left, const Tuple &right, integer_sequence<size_t, Is...>) {
    uint16_t header = 0;
    expander{ 0, ((void)diffHeaderIter<Tuple, Is>(left, right, header), 0) ... }; // pseudo fold-expansion
    return header;
}

template <class Tuple, size_t I>
void writeDeltaIter(StreamWriter &writer, const Tuple &o, uint16_t &header) {
    if (header  &(1 << I)) {
        writer << get<I>(o);
    }
}

template <class Tuple, size_t ... Is>
void writeDeltaImpl(StreamWriter &writer, const Tuple &o, uint16_t &header, integer_sequence<size_t, Is...>) {
    expander{ 0, ((void)writeDeltaIter<Tuple, Is>(writer, o, header), 0) ... }; // pseudo fold-expansion
}


template <class TuplePtr, size_t I>
void readDeltaIter(StreamReader &reader, TuplePtr &optr, uint16_t &header) {
    if (header  &(1 << I)) {
        reader >> *(get<I>(optr));
    }
}

template <class TuplePtr, size_t ... Is>
void readDeltaImpl(StreamReader &reader, TuplePtr &&optr, uint16_t &header, integer_sequence<size_t, Is...>) {
    expander{ 0, ((void)readDeltaIter<TuplePtr, Is>(reader, optr, header), 0) ... };
}

/* END helper functions */

template <class Tuple>
void writeDeltaWithHeader(const Tuple &cur, const Tuple &old, StreamWriter &writer) {
    uint16_t header = diffHeader(cur, old, make_index_sequence<tuple_size<Tuple>::value>());
    writer << header;
    writeDeltaImpl(writer, cur, header, make_index_sequence<tuple_size<Tuple>::value>());
}

template <class T>
void writeDelta(StreamWriter &writer, T &cur, T &old) {
    writeDeltaWithHeader(getSerial(cur), getSerial(old), writer);
}

/* r-value to discourage outside scope usage*/
template <class TuplePtr>
void readDeltaWithHeader(TuplePtr &&obj, StreamReader &reader) {
    uint16_t header = 0;
    reader >> header;
    readDeltaImpl(reader, move(obj), header, make_index_sequence<tuple_size<TuplePtr>::value>());
}

template <class T>
void readDelta(StreamReader &reader, T &obj) {
    readDeltaWithHeader(getSerialPointers(obj), reader);
}

// https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
#define DECL_SERIAL(T, ...) auto getSerialPointers(T &obj) \
    { return make_tuple(__VA_ARGS__); } \
    template void writeDelta<T>(StreamWriter &writer, T &cur, T &old); \
    template void readDelta<T>(StreamReader &reader, T &obj);

DECL_SERIAL(CreatureStatus, &obj.x, &obj.y, &obj.z, &obj.heading, &obj.anim, &obj.animframe, &obj.faction);

// WARNING: danger of dangling, can only be used in the context of 
// member function
// auto getSerialPointers(CreatureStatus &obj) {
//     return make_tuple(
//         &obj.x,
//         &obj.y,
//         &obj.z,
//         &obj.heading,
//         &obj.anim,
//         &obj.animframe,
//         &obj.faction
//     );
// }

// template void writeDelta<CreatureStatus>(PacketWriter &writer, CreatureStatus &cur, CreatureStatus &old);
// template void readDelta<CreatureStatus>(PacketReader &reader, CreatureStatus &obj);

} // namespace net

} // namespace reone
