#ifndef ___SKIP_LIST_HPP
#define ___SKIP_LIST_HPP

#include <iostream>
#include <cmath>  // for log2
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace shindler::ics46::project2 {

/**
 * flipCoin -- NOTE: Only read if you are interested in how the
 * coin flipping works.
 *
 * @brief Uses the bitwise representation of the key to simulate
 * the flipping of a deterministic coin.
 *
 * This function looks at the bitwise representation to determine
 * how many layers it occupies in the skip list. It takes the bitwise
 * XOR of each byte in a 32-bit unsigned number and uses the index
 * `previousFlips` to determine the truth value. It's best to look at an example
 * and to start with values that fit into one byte.
 *
 * Let's start with the function call `flipCoin(0, 0)`. This call is
 * asking whether or not we should insert the key `0` into layer `1`
 * (because `previousFlips` represents the number of previous flips).
 *
 * This function will get the bitwise representation of 0:
 *
 *        00000000 00000000 00000000 00000000
 *
 * All bytes are XOR'd together:
 *
 *
 *  c = 0 ^ 0 ^ 0 ^ 0
 *
 * Then the index `previousFlips` is used to obtain the bit in the
 * `previousFlips % 8` position.
 *
 * So the return value is `0 & 1 = 0`, since the value in the zeroth position
 * (obtained by bitmasking the value (1 << 0) at the bottom of the function
 * is 0.
 * Thus, this value `0` should never get added beyond the bottom-most layer.
 *
 * Before:
 *
 * S_1: -inf ----> inf
 * S_0: -inf ----> inf
 *
 * And after 0 is inserted
 *
 * S_1: -inf --------> inf
 * S_0: -inf --> 0 --> inf
 *
 * Let's look at something more interesting, like the call `flipCoin(5, 0)`.
 * Remember the binary representation for 5 is 00000101.
 *
 * c = 0 ^ 0 ^ 0 ^ 00000101 = 00000101
 *
 * Now we get the bit at 0th position (from `previousFlips`).
 *
 * 00000101 & 1 = 1. Thus, `5` DOES get inserted into the next layer,
 * layer 1.
 *
 * So the skip list before this insertion might look like:
 *
 * S_1: -inf ----> inf
 * S_0: -inf ----> inf
 *
 * And after it would look like:
 *
 * S_2: -inf --------> inf
 * S_1: -inf --> 5 --> inf
 * S_0: -inf --> 5 --> inf
 *
 * Observe that a new layer had to get created, because there should always
 * be an empty layer at the top.
 *
 * Since 5 got inserted into the next layer, we need to flip again to see if
 * it should get propagated once more.
 *
 * `flipCoin(5, 1)` this will produce a result of `0`, since 00000101 & (1 << 1)
 * = 0.
 *
 * Thus, `5` will not be pushed up to the next layer.
 *
 * @param key The integer key which will be added to the skip list
 * @param previousFlips The number of previous flips for this key
 * @return true simulates a "heads" from a coin flip
 * @return false simulates a "tails" from a coin flip
 */
enum class IntegerByteOffsets {
    Byte0 = 24,
    Byte1 = 16,
    Byte2 = 8,
    Byte3 = 0,
};

constexpr inline std::byte flipCoinByteSelector(uint32_t key,
                                                IntegerByteOffsets byte) {
    const uint32_t BYTE_SELECTOR{0xFF};
    auto byteAsInteger{static_cast<uint32_t>(byte)};
    return static_cast<std::byte>((key & (BYTE_SELECTOR << byteAsInteger)) >>
                                  byteAsInteger);
}

const uint32_t NUMBER_OF_BITS_IN_BYTE = 8;

constexpr inline bool flipCoin(unsigned int key, size_t previousFlips) {
    std::byte firstByte{flipCoinByteSelector(key, IntegerByteOffsets::Byte0)};
    std::byte secondByte{flipCoinByteSelector(key, IntegerByteOffsets::Byte1)};
    std::byte thirdByte{flipCoinByteSelector(key, IntegerByteOffsets::Byte2)};
    std::byte fourthByte{flipCoinByteSelector(key, IntegerByteOffsets::Byte3)};

    std::byte hash{firstByte ^ secondByte ^ thirdByte ^ fourthByte};

    auto bitToSelect = std::byte{
        static_cast<uint8_t>(1 << (previousFlips % NUMBER_OF_BITS_IN_BYTE))};

    return std::to_integer<uint8_t>(hash & bitToSelect) != 0;
}

/**
 * @brief Works the same as the integer version above, except
 * it XORs chars in a string instead of the first four
 * bytes of an integer.
 *
 * @param key key that will be inserted into the skip list
 * @param previousFlips number of previous flips for this key
 * @return true simulates a "heads" from a coin flip
 * @return false simulates a "tails" from a coin flip
 */
constexpr inline bool flipCoin(const std::string& key, size_t previousFlips) {
    std::byte hash{};

    for (auto character : key) {
        hash ^= static_cast<std::byte>(character);
    }

    std::byte bitToSelect{
        static_cast<uint8_t>(1 << (previousFlips % NUMBER_OF_BITS_IN_BYTE))};

    return std::to_integer<uint8_t>(hash & bitToSelect) != 0;
}

template <typename K, typename V>
class SkipList {
   private:
   size_t SkipListSize{0};
   size_t SkipListLayers{0};
   struct Node
   {
    Node(K k, V v)
    :key{k}, value{v}, next{nullptr}, up{nullptr}, down{nullptr}, previous{nullptr}
    {
    }
    K key;
    V value;
    Node * next{nullptr};
    Node * up{nullptr};
    Node * down{nullptr};
    Node * previous{nullptr};
   };
   Node * front{};
   Node * back{};
   Node * topFront{};
   Node * topBack{};

    // private variables go here.

   public:
    SkipList();

    void printSkipList() const;

    Node* findNode(const K& key);
    Node* findNode(const K& key) const;


    // You DO NOT need to implement a copy constructor or an assignment
    // operator. These are only here to make sure that the compiler stops
    // any unsopported behavior. For those of you that took 45c last quarter
    // you'll probably remember the incident where the copy was not implement
    // and it was doing a shallow copy which caused a bunch of issues. This
    // will prevent that (in theory).
    SkipList(const SkipList&) = delete;
    SkipList(SkipList&&) = delete;
    SkipList& operator=(const SkipList&) = delete;
    SkipList& operator=(SkipList&&) = delete;

    ~SkipList();

    // How many distinct keys are in the skip list?
    [[nodiscard]] size_t size() const noexcept;

    // Does the Skip List contain zero keys?
    [[nodiscard]] bool empty() const noexcept;

    // How many layers are in the skip list?
    // Note that an empty Skip List has two layers by default,
    // the "base" layer S_0 and the top layer S_1.
    //
    // [S_1: Top layer]    -inf ------> inf
    // [S_0: Bottom layer] -inf ------> inf
    //
    // This "empty" Skip List has two layers and a height of one.
    [[nodiscard]] size_t layers() const noexcept;

    // What is the height of this key, assuming the "base" layer S_0
    // contains keys with a height of 1?
    // For example, "0" has a height of 1 in the following skip list.
    //
    // [S_1]  -inf --------> inf
    // [S_0]  -inf --> 0 --> inf
    //
    // Throw an exception if this key is not in the Skip List.
    [[nodiscard]] size_t height(const K& key) const;

    // If this key is in the SkipList and there is a next largest key
    // return the next largest key.
    // This function should throw a std::out_of_range if either the key doesn't
    // exist or there is no subsequent key. A consequence of this is that this
    // function will throw a std::out_of_range if *k* is the *largest* key in
    // the Skip List.
    [[nodiscard]] const K& nextKey(const K& key) const;
    [[nodiscard]] const K& previousKey(const K& key) const;

    // These return the value associated with the given key.
    // Throw a std::out_of_range if the key does not exist.
    [[nodiscard]] V& find(const K& key);
    [[nodiscard]] const V& find(const K& key) const;

    // Return true if this key/value pair is successfully inserted, false
    // otherwise. See the project write-up for conditions under which the key
    // should be "bubbled up" to the next layer. If the key already exists, do
    // not insert one -- return false.
    bool insert(const K& key, const V& value);

    // Return a vector containing all inserted keys in increasing order.
    [[nodiscard]] std::vector<K> allKeysInOrder() const;

    // Is this the smallest key in the SkipList? Throw a std::out_of_range
    // if the key *k* does not exist in the Skip List.
    [[nodiscard]] bool isSmallestKey(const K& key) const;

    // Is this the largest key in the SkipList? Throw a std::out_of_range
    // if the key *k* does not exist in the Skip List.
    [[nodiscard]] bool isLargestKey(const K& key) const;

    // Erase the given key from the skip list. Throw a std::out_of_range
    // if the key *key* does not exist in the SkipList
    void erase(const K& key);
};

template <typename K, typename V>
SkipList<K, V>::SkipList() 
{
    //Intialize the intial two layer lists.
    
    this -> front = new Node({}, {});
    this -> back = new Node({}, {});
    this -> topFront = new Node({}, {});
    this -> topBack = new Node({}, {});

    //Sets front's next and up nodes should have nullptr for down and previous
    this -> front -> up = this -> topFront;
    this -> front -> next = this -> back;
    //Sets back's previous and up nodes should have nullptr for next and down
    this -> back -> previous = this -> front;
    this -> back -> up = this -> topBack;
    //Sets topFront's down and next nodes, should have nullptr for previous and up
    this -> topFront -> down = this -> front;
    this -> topFront -> next = this -> topBack;
    //Sets topBack's previous and down nodes, should have nulltr up and next.
    this -> topBack -> previous = this -> topFront;
    this -> topBack -> down = this -> back;
    //Initialize SkipListLayers to deflaut value of 2
    SkipListLayers += 2;

    
}

template <typename K, typename V>
void SkipList<K, V>::printSkipList() const {
    Node* current = topFront;

    while (current != nullptr) {
        Node* tmp = current;

        while (tmp != nullptr) {
            std::cout << tmp->key << " ";
            tmp = tmp->next;
        }

        std::cout << "\n";
        current = current->down;
    }
    std::cout << "------------------------\n";
}


template <typename K, typename V>
SkipList<K, V>::~SkipList() {
    Node* current = topFront;
    
    while (current != nullptr) {
        Node* nextLayer = current->down;
        Node* temp;
        
        // Delete all nodes in the current layer
        while (current != nullptr) {
            temp = current;
            current = current->next;
            delete temp;
        }
        
        current = nextLayer;
    }
    
    // Reset pointers after all nodes are deleted
    front = back = topFront = topBack = nullptr;
}

template <typename K, typename V>
size_t SkipList<K, V>::size() const noexcept {
    return SkipListSize;
}

template <typename K, typename V>
bool SkipList<K, V>::empty() const noexcept {
    return (SkipListSize == 0);
}

template <typename K, typename V>
size_t SkipList<K, V>::layers() const noexcept {
    return SkipListLayers;
}

template <typename K, typename V>
size_t SkipList<K, V>::height(const K& key) const {
    size_t layers{1};
    Node * tmp = findNode(key);
    while (tmp -> up != nullptr)
    {
        layers++;
        tmp = tmp -> up;
    }
    return layers;
}

template <typename K, typename V>
const K& SkipList<K, V>::nextKey(const K& key) const {
    // TODO - your implementation goes here!
    Node * tmp{findNode(key)};
    if (tmp -> next -> next == nullptr)
    {
        throw std::runtime_error("ERROR");
    }
    return tmp -> next -> key;
}

template <typename K, typename V>
const K& SkipList<K, V>::previousKey(const K& key) const {
    Node * tmp{findNode(key)};
    if (tmp -> previous -> previous == nullptr)
    {
        throw std::runtime_error("ERROR");
    }
    return tmp -> previous -> key;
}

template <typename K, typename V>
typename SkipList<K, V>::Node* SkipList<K, V>::findNode(const K& key){
    Node * tmp{this -> topFront}; // We will start the skip list finding feature from the top
    while (tmp -> down != nullptr) // Go until tmp -> down is a nullptr which means that it is at the base layer and cannot go anymore
    {
        if (tmp -> next -> next != nullptr and tmp -> next -> key < key) //Tmp -> next -> next is used to find the tail since the tail will be the only one with a next nullptr
        {
            tmp = tmp -> next;
        }
        else
        {
            tmp = tmp -> down; //If tmp cannot find a value that matches the criteria then tmp will just go down
        }
    }
    while (tmp -> next -> next != nullptr) // Will keep going until the next value is back or next value's key is greater than the key
    {
        tmp = tmp -> next;
        if (tmp -> key == key)
        {   
            return tmp;
        }
    }
    throw std::out_of_range("Error");
}

template <typename K, typename V>
typename SkipList<K, V>::Node* SkipList<K, V>::findNode(const K& key) const{
    Node * tmp{this -> topFront}; // We will start the skip list finding feature from the top
    while (tmp -> down != nullptr) // Go until tmp -> down is a nullptr which means that it is at the base layer and cannot go anymore
    {
        if (tmp -> next -> next != nullptr and tmp -> next -> key < key) //Tmp -> next -> next is used to find the tail since the tail will be the only one with a next nullptr
        {
            tmp = tmp -> next;
        }
        else
        {
            tmp = tmp -> down; //If tmp cannot find a value that matches the criteria then tmp will just go down
        }
    }
    while (tmp -> next -> next != nullptr) // Will keep going until the next value is back or next value's key is greater than the key
    {
        tmp = tmp -> next;
        if (tmp -> key == key)
        {   
            return tmp;
        }
    }
    throw std::out_of_range("Error");
}
template <typename K, typename V>
const V& SkipList<K, V>::find(const K& key) const {
    
    return findNode(key) -> value;

}

template <typename K, typename V>
V& SkipList<K, V>::find(const K& key) {
    return findNode(key) -> value;
}

template <typename K, typename V>
bool SkipList<K, V>::insert(const K& key, const V& value) {
    Node * tmp{this -> topFront}; // We will start the skip list finding feature from the top
    while (tmp -> down != nullptr) // Go until tmp -> down is a nullptr which means that it is at the base layer and cannot go anymore
    {
        if (tmp -> next -> next != nullptr and tmp -> next -> key < key) //Tmp -> next -> next is used to find the tail since the tail will be the only one with a next nullptr
        {
            tmp = tmp -> next; // Set the tmp to the next node if the key is not at the back and the key is smaller than input key.
        }
        else
        {
            tmp = tmp -> down; //If tmp cannot find a value that matches the criteria then tmp will just go down
        }
    }

    while (tmp -> next -> next != nullptr and tmp -> next -> key < key) // Will keep going until the next value is back or next value's key is greater than the key
    {
        if(tmp -> key == key)
        {
            return false;
        }
        tmp = tmp -> next;
    } 

    Node * newNode = new Node(key, value); //Create a new node that we will connect to this point.

    newNode -> previous = tmp; //Connects newNode's previous to tmp since tmp is the value that is smaller than it. Connect newNode's next to the value that would have been bigger which is next.
    newNode -> next = tmp -> next;

    //Connect the old nodes next and  previous to newNode
    tmp -> next -> previous = newNode;
    tmp -> next = newNode;
    SkipListSize++;

    
    //Began randomize process
    size_t numberOfFlips{0};
    Node * randTmp{tmp}; //Will be used as a tmp random process
    Node * randTmp2{tmp -> next}; //Store value for the key
    size_t layers{1};
    while (flipCoin(key, numberOfFlips))
    {

        //Check if layers are about to be exceeded add a new 
        if (layers == SkipListLayers - 1)
        {
            Node * newTop = new Node({}, {});
            Node * newTopBack = new Node({}, {});

            //Connect the new layers with each other
            newTop -> down = this -> topFront;
            newTopBack -> down = this -> topBack;
            newTop -> next = newTopBack;
            newTopBack -> previous = newTop;

            //Connect previous node to new nodes
            this -> topFront -> up = newTop;
            this -> topBack -> up = newTopBack;

            this -> topFront = newTop;
            this -> topBack = newTopBack;
            SkipListLayers++;
        }

        
        while (randTmp -> up == nullptr) //If it is null ptr then it cannot be the value that led to TMP
        {
            randTmp = randTmp -> previous;
        }
        
        Node * newLayer = new Node(key, value);

        randTmp = randTmp -> up;

        //Connect nodes for newLayer
        newLayer -> previous = randTmp;
        newLayer -> down = randTmp2;
        newLayer -> next = randTmp -> next;
        
        //Connect the key node to this new layer node
        randTmp2 -> up = newLayer;
        randTmp -> next -> previous = newLayer;
        randTmp -> next = newLayer;

        //Set the new layer node to randTmp2 so if it goes up again this is the one that it will connect to.
        randTmp2 = newLayer;
        
        if (SkipListSize <= 16)
        {
            if (SkipListLayers == 13)
            {
                break;
            }
        }
        if (SkipListSize > 16)
        {
            if (SkipListLayers == (3 * ceil(log2(SkipListSize)) + 1))
            {
                break;
            }
        }
        layers++;
        numberOfFlips++;
    }
    return true;
}

template <typename K, typename V>
std::vector<K> SkipList<K, V>::allKeysInOrder() const {
    std::vector<K> keys{}; //Empty Vector

    Node * tmp {this -> front -> next}; //Make node pointer to the first value after front

    while (tmp != this -> back)
    {
        keys.push_back(tmp -> key);
        tmp = tmp -> next;
    }
    
    return keys;
}

template <typename K, typename V>
bool SkipList<K, V>::isSmallestKey(const K& key) const {
    findNode(key);
    return (this -> front -> next -> key == key);
}

template <typename K, typename V>
bool SkipList<K, V>::isLargestKey(const K& key) const {
    findNode(key);
    return (this -> back -> previous -> key == key);
}

template <typename K, typename V>
void SkipList<K, V>::erase(const K& key) {
    Node * tmp{findNode(key)}; //Find the node that this value is at
    while (tmp != nullptr)
    {
        Node * tmpPrevious{tmp -> previous};
        Node * tmpNext{tmp -> next};
        //Store the nodes next and previous values so can connect them

        tmpPrevious -> next = tmpNext;
        tmpNext -> previous = tmpPrevious;

        Node * deleteNode{tmp}; //Keep track so can delete
        tmp = tmp -> up;
        delete deleteNode;
    }
    SkipListSize--;
}

}  // namespace shindler::ics46::project2
#endif
