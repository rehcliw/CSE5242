const unsigned int CUCKOO_MAX_LOOP = 128;

void CuckooHashTable::displace(void* tupstore, unsigned int loopno)
{
	if (loopno > CUCKOO_MAX_LOOP) {
		throw CuckooHashTableFull();
	}

	void* h1Space = allocateInBucket(storehasher[0].hash(tupstore));
	if (h1Space != NULL) {
		tupstore = h1Space;
		return;
	}

	void* h2Space = allocateInBucket(storehasher[1].hash(tupstore));
	if (h2Space != NULL) {
		tupstore = h2Space;
		return;
	}

	VictimT victim = pickVictim();
	void* victimSpace;
	victimSpace = calcTupleLocation(storehasher[victim.hashno].hash(tupstore), victim.slot);

	displace(victimSpace, loopno + 1);
}

void* CuckooHashTable::allocate(void* tupbuild, void* allocsource)
{	
	void* h1Space = allocateInBucket(buildhasher[0].hash(tupbuild));
	if (h1Space != NULL) {
		return h1Space;
	}

	void* h2Space = allocateInBucket(buildhasher[1].hash(tupbuild));
	if (h2Space != NULL) {
		return h2Space;
	}
	
	VictimT victim = pickVictim();
	void* victimSpace;
	
	victimSpace = calcTupleLocation(buildhasher[victim.hashno].hash(tupbuild), victim.slot);

	displace(victimSpace, 0);

	return victimSpace;
}

