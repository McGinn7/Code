const int N = 50000 + 10;
const int D = 5;
const double SCALE = 0.75;
struct Point { int x[D]; } buf[N];
int d, ctr;
long long ret;
int cmp(const Point &a, const Point &b) { return a.x[ctr] < b.x[ctr]; }
struct Node {
	int depth, size;
	Node *ch[2], *p;
	Point val, maxv, minv;
	void set(Node *t, int d) { ch[d] = t; t->p = this; }
	bool dir() { return this == p->ch[1]; }
	bool balanced() {
		return (double)max(ch[0]->size, ch[1]->size) <= (double)size * SCALE;
	}
	void update() {
		size = ch[0]->size + ch[1]->size + 1;
		for(int i = 0; i < d; ++ i) {
			maxv.x[i] = max(val.x[i], max(ch[0]->maxv.x[i], ch[1]->maxv.x[i]));
			minv.x[i] = min(val.x[i], min(ch[0]->minv.x[i], ch[1]->minv.x[i]));
		}
	}
} nodePool[N], *totNode, *null;
Node* newNode(Point p, int depth) {
	Node *t = totNode++;
	t->size = 1;
	t->depth = depth;
	t->val = t->maxv = t->minv = p;
	t->ch[0] = t->ch[1] = t->p = null;
	return t;
}
struct KDTree {
	Node *root;
	Node *build(Point *a, int l, int r, int depth) {
		if (l > r) return null;
		ctr = depth;
		sort(a + l, a + r + 1, cmp);
		int mid = (l + r) >> 1;
		Node *t = newNode(a[mid], depth);
		t->set(build(a, l, mid - 1, (depth + 1) % d), 0);
		t->set(build(a, mid + 1, r, (depth + 1) % d), 1);
		t->update();
		return t;
	}
	void tranverse(Node *t, Point *vec, int &tot) {
		if (t == null) return;
		vec[tot ++] = t->val;
		tranverse(t->ch[0], vec, tot);
		tranverse(t->ch[1], vec, tot);
	}
	void rebuild(Node *t) {
		Node *p = t->p;
		int tot = 0;
		tranverse(t, buf, tot);
		Node *u = build(buf, 0, tot - 1, t->depth);
		p->set(u, t->dir());
		for( ; p != null; p = p->p) p->update();
		if (t == root) root = u;
	}
	void insert(Point p) {
		if (root == null) { root = newNode(p, 0); return; }
		Node *cur = root, *last = null;
		int dir = 0;
		for( ; cur != null; ) {
			last = cur;
			dir = (p.x[cur->depth] > cur->val.x[cur->depth]);
			cur = cur->ch[dir];
		}
		Node *t = newNode(p, (last->depth + 1) % d), *bad = null;
		last->set(t, dir);
		for( ; t != null; t = t->p) {
			t->update();
			if (!t->balanced()) bad = t;
		}
		if (bad != null) rebuild(bad);
	}
	long long calcEval(Point u, Node *t, int d) {
		long long l = t->minv.x[d], r = t->maxv.x[d], x = u.x[d];
		if (x >= l && x <= r) return 0LL;
		long long ret = min(abs(x - l), abs(x - r));
		return ret * ret;
	}
	
	struct Cmp {
		bool operator()(const pair<int, Point> &a, const pair<int, Point> &b) {
			return a.fi < b.fi;	
		}
	};
	// get the nearest MS points
	int MS;
	priority_queue<pair<int, Point>, vector<pair<int, Point> >, Cmp > ans;

	void updateAns(Point u, Point p) {
		int dis = 0;
		rep(i, 0, K) dis += (u.x[i] - p.x[i]) * (u.x[i] - p.x[i]);
		if (sz(ans) < MS) {
			ans.push(mp(dis, u));
		} else if (dis < ans.top().fi) {
			ans.pop();
			ans.push(mp(dis, u));
		}
	}
	void query(Node *t, Point p) {
		if (t == null) return;
		updateAns(t->val, p);
		long long evalLeft = calcEval(p, t->ch[0], t->depth);
		long long evalRight = calcEval(p, t->ch[1], t->depth);	
		if (evalLeft <= evalRight) {
			query(t->ch[0], p);
			if (sz(ans) < MS || ans.top().fi > evalRight) query(t->ch[1], p);
		} else {
			query(t->ch[1], p);
			if (sz(ans) < MS || ans.top().fi > evalLeft) query(t->ch[0], p);
		}
	}
	void query(Point p) {
		query(root, p);
	}
} kd;
void initNull(int _d) {
	d = _d;
	totNode = nodePool;
	null = totNode ++;
	null->size = 0;
	for(int i = 0; i < d; ++ i) {
		null->maxv.x[i] = -INF;
		null->minv.x[i] = INF;
	}
}

