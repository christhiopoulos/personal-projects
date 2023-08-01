# Κάνοντας compile το <foo>_test.c με μια υλοποίηση <foo>.c του
# συγκεκριμένου τύπου, παράγουμε ένα tets για την υλοποίηση αυτή.

# Υλοποιήσεις μέσω Heap: ADTPriorityQueue
# Το Heap χρησιμοποιεί Vector, οπότε χρειαζόμαστε και μια υλοποίηση του ADTVector.
# Ο βασικός κορμός του Makefile

UsingLinkedTree_ADTRecTree_test_OBJS=ADTRecTree_test.o   $(MODULES)/UsingLinkedTree/ADTRecTree.o  


UsingMap_ADTRecTree_test_OBJS=ADTRecTree_test.o	  $(MODULES)/UsingADTMap/ADTRecTree.o   $(MODULES)/UsingADTMap/ADTSet.o  $(MODULES)/UsingADTMap/ADTMap.o


UsingMap_ADTRecTree_utils_test.o_OBJS= ADTRecTree_utils_test.o	$(MODULES)/UsingADTMap/ADTMap.o 	 $(MODULES)/UsingADTMap/ADTRecTree.o     $(MODULES)/ADTRecTree_utils.o   $(MODULES)/UsingADTMap/ADTSet.o 


UsingLinkedTree_ADTRecTree_utils_test.o_OBJS= ADTRecTree_utils_test.o   $(MODULES)/UsingLinkedTree/ADTRecTree.o    $(MODULES)/ADTRecTree_utils.o   


UsingLinkedTree_ADTCompTree_test_OBJS=ADTCompTree_test.o    $(MODULES)/UsingLinkedTree/ADTRecTree.o    $(MODULES)/ADTRecTree_utils.o   $(MODULES)/UsingADTRecTree/ADTCompTree.o


UsingMap_UsingLinkedTree_ADTCompTree_test_OBJS=ADTCompTree_test.o    $(MODULES)/ADTRecTree_utils.o   $(MODULES)/UsingADTRecTree/ADTCompTree.o    $(MODULES)/UsingADTMap/ADTMap.o  $(MODULES)/UsingADTMap/ADTRecTree.o   $(MODULES)/UsingADTMap/ADTSet.o 


UsingHeap_ADTPriorityQueue_usingADTMap_test_OBJS  = ADTPriorityQueue_test.o   $(MODULES)/UsingLinkedTree/ADTRecTree.o    $(MODULES)/ADTRecTree_utils.o   $(MODULES)/UsingADTRecTree/ADTCompTree.o   $(MODULES)/UsingADTMap/ADTSet.o  $(MODULES)/UsingCompTree/ADTPriorityQueue.o $(MODULES)/UsingDynamicArray/ADTVector.o


UsingHeap_ADTPriorityQueue_usingADTLinkedTree_test_OBJS = ADTPriorityQueue_test.o  $(MODULES)/UsingADTMap/ADTMap.o   $(MODULES)/UsingADTMap/ADTRecTree.o   $(MODULES)/ADTRecTree_utils.o      $(MODULES)/UsingADTRecTree/ADTCompTree.o   $(MODULES)/UsingADTMap/ADTSet.o   $(MODULES)/UsingCompTree/ADTPriorityQueue.o $(MODULES)/UsingDynamicArray/ADTVector.o



include ../common.mk