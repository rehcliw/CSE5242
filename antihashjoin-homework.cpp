
Operator::GetNextResultT AntiHashJoinOp::getNext(unsigned short threadid)
{
	void* tup1;
	void* tup2;


	Page* out = output[threadid];
	HashJoinState* state = hashjoinstate[threadid];
	dbgassert(state->htiter != NULL);
	GeneralHashTable* ht = hashtable[threadgroups[threadid]];

	out->clear();
	tup2 = state->location;

	// Reposition based on iterators.
	while(out->canStoreTuple()) 
	{
		bool matchFound = false;
		void* target;
		// Finish joining last tuple.
		while ( (tup1 = state->htiter->next()) ) 
		{	
			if (keycomparator.eval(tup1, tup2) == true) 
			{
				matchFound = true;
			}
			continue;
		}
		if (matchFound == false)
 		{ 
			// Keys unequal, join tup1 with tup2 and copy at output buffer.
			target = out->allocateTuple();
			dbg2assert(target!=NULL);

			constructOutputTuple(tup1, tup2, target);
		}
		// advance *tup2 to nxt tuple from probe side
		tup2 = readNextTupleFromProbe(threadid); // set *tup2 to next tuple from probe input
		state->location = tup2;

		delete state->htiter;
		state->htiter = ht->createKeyIterator();
		return make_pair(Operator::Finished, out);

		ht->placeKeyIterator(*state->htiter, tup2);
	}
	return make_pair(Ready, out);
}
