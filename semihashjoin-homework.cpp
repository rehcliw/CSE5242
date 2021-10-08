
Operator::GetNextResultT SemiHashJoinOp::getNext(unsigned short threadid)
{
	void* tup1; // *tup1 is a tuple in the hash table
	void* tup2; // * tup2 is a tuple in the probe input


	Page* out = output[threadid];
	HashJoinState* state = hashjoinstate[threadid];
	dbgassert(state->htiter != NULL);
	GeneralHashTable* ht = hashtable[threadgroups[threadid]];

	out->clear(); // clear output buffer for reuse
	tup2 = state->location; // reset *tup2 to last processed probe tuple

	// while output buffer can store more tuples
	while(out->canStoreTuple()) 
	{
		// for each tuple *tup1 from the hash table iterator
		while ( (tup1 = state->htiter->next()) ) 
		{
			void* target;

			if (keycomparator.eval(tup1, tup2) == false) 
			{
				continue;
			}

			// Keys equal, join tup1 with tup2 and copy at output buffer.
			target = out->allocateTuple(); 
			dbg2assert(target!=NULL);

			constructOutputTuple(tup1, tup2, target);

			// advance *tup2 to next tuple from probe side
			tup2 = readNextTupleFromProbe(threadid);
			state->location = tup2; // remember last processed probe tuple is *tup2

			// if probe input depeleted, stop operation cleanly
			delete state->htiter;
			state->htiter = ht->createKeyIterator();
			// return output buffer with Finished
			return make_pair(Operator::Finished, out);
			
			ht->placeKeyIterator(*state->htiter,tup2);
		}
	}
	return make_pair(Ready, out); // Return the output buffer with Ready	
}
