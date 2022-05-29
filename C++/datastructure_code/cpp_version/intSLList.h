//************************  intSLList.h  **************************
// ������ ʵ���� 

#ifndef INT_LINKED_LIST
#define INT_LINKED_LIST

// �����ڵ�
// ����һ�� �ڵ�ָ��
// IntLLNode* pt = new IntLLNode(10);//�½�һ���ڵ� ���ַ ����һ��ָ�� IntLLNode ��ָ�� pt
//
//   [pt]  ----> |  10 | Ҳ���� pt->info_   Ҳ���� (*pt).info_
//               |  \  | Ҳ���� pt->nextPtr_ Ҳ���� (*pt).nextPtr_  
class IntSLLNode {
public:
    IntSLLNode() {// Ĭ�Ϲ��캯��   û��info��Ϣ
        next = 0;// ��ָ��
    }
    IntSLLNode(int data, IntSLLNode *ptr = 0) {// �ڶ������캯��
        info = data; 
		next = ptr;
    }
public:    
    int info;         // ����һ����Ϣ          ���û�����Ҫ
    IntSLLNode *next; // ָ����һ�� �ڵ��ָ��  ���ڽ��ڵ����������������
};

// ���� ��  ������һ�� ͷ�ڵ� �� һ��β�ڵ� 
class IntSLList {
public:
    IntSLList() {// Ĭ�Ϲ��캯��  ���ﶨ���ʵ�� 
        head = tail = 0;// �ڵ� �� һ��β�ڵ� ָ�븳ֵΪ �� 
    }
    ~IntSLList();// Ĭ����������  ֻ�ж��� ʵ���� cpp�ļ��� 
    int isEmpty() {//Ϊ�������Ƿ� 
        return head == 0;
    }
    void addToHead(int);   // ��ͷ����� �ڵ� 
    void addToTail(int);   // ��β����� �ڵ� 
    int  deleteFromHead(); // ��ͷ��ɾ�� �ڵ� �����ظýڵ����Ϣ 
    int  deleteFromTail(); // ��β��ɾ�� �ڵ� �����ظýڵ����Ϣ 
    void deleteNode(int);//ɾ�� 
    bool isInList(int) const;
    void printAll() const;//��ӡ���еĽڵ����Ϣ 
    
    
private:
    IntSLLNode *head, *tail;
};

#endif
