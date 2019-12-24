#if GS_DELEGATE_NUM_ARGS == 0
#define GS_DELEGATE_COMMA
#else
#define GS_DELEGATE_COMMA    ,
#endif

//-------------------------------------------------------------------------
// class Delegate<R (T1, T2, ..., TN)>

template <class R GS_DELEGATE_COMMA GS_DELEGATE_TEMPLATE_PARAMS>
class GsDelegate<R (GS_DELEGATE_TEMPLATE_ARGS)>
{
// Declaractions
private:
    class DelegateImplBase
    {
    // Fields
    public:
        DelegateImplBase* Previous; // singly-linked list

    // Constructor/Destructor
    protected:
        DelegateImplBase() : Previous(NULL) { }
        DelegateImplBase(const DelegateImplBase& other) : Previous(NULL) { }
    public:
        virtual ~DelegateImplBase() { }

    // Methods
    public:
        virtual DelegateImplBase* Clone() const = 0;
        virtual R Invoke(GS_DELEGATE_FUNCTION_PARAMS) const = 0;
    };

    template <class TFunctor>
    struct Invoker
    {
        static R Invoke(const TFunctor& f GS_DELEGATE_COMMA GS_DELEGATE_FUNCTION_PARAMS)
        {
            return (const_cast<TFunctor&>(f))(GS_DELEGATE_FUNCTION_ARGS);
        }
    };

    template <class TPtr, class TFunctionPtr>
    struct Invoker<std::pair<TPtr, TFunctionPtr> >
    {
        static R Invoke(const std::pair<TPtr, TFunctionPtr>& mf GS_DELEGATE_COMMA GS_DELEGATE_FUNCTION_PARAMS)
        {
            return ((*mf.first).*mf.second)(GS_DELEGATE_FUNCTION_ARGS);
        }
    };

    template <class TFunctor>
    class DelegateImpl : public DelegateImplBase
    {
    // Fields
    public:
        TFunctor Functor;

    // Constructor
    public:
        DelegateImpl(const TFunctor& f) : Functor(f)
        {
        }
        DelegateImpl(const DelegateImpl& other) : Functor(other.Functor)
        {
        }

    // Methods
    public:
        virtual DelegateImplBase* Clone() const
        {
            return new DelegateImpl(*this);
        }
        virtual R Invoke(GS_DELEGATE_FUNCTION_PARAMS) const
        {
            return Invoker<TFunctor>::Invoke(this->Functor GS_DELEGATE_COMMA GS_DELEGATE_FUNCTION_ARGS);
        }
    };

// Fields
private:
    DelegateImplBase* _last;

// Constructor/Destructor
public:
    GsDelegate()
    {
        this->_last = NULL;
    }

    template <class TFunctor>
    GsDelegate(const TFunctor& f)
    {
        this->_last = NULL;
        *this = f;
    }

	template<class TPtr, class TFunctionPtr>
    GsDelegate(const TPtr& obj, const TFunctionPtr& mfp)
    {
        this->_last = NULL;
        *this = std::make_pair(obj, mfp);
    }

    GsDelegate(const GsDelegate& d)
    {
        this->_last = NULL;
        *this = d;
    }

    ~GsDelegate()
    {
        Clear();
    }

// Properties
public:
    bool IsEmpty() const
    {
        return (this->_last == NULL);
    }

    bool IsMulticast() const
    {
        return (this->_last != NULL && this->_last->Previous != NULL);
    }

// Static Methods
private:
    static DelegateImplBase* CloneDelegateList(DelegateImplBase* list, /*out*/ DelegateImplBase** first)
    {
        DelegateImplBase* list2 = list;
        DelegateImplBase* newList = NULL;
        DelegateImplBase** pp = &newList;
        DelegateImplBase* temp = NULL;

        try
        {
            while (list2 != NULL)
            {
                temp = list2->Clone();
                *pp = temp;
                pp = &temp->Previous;
                list2 = list2->Previous;
            }
        }
        catch (...)
        {
            FreeDelegateList(newList);
            throw;
        }

        if (first != NULL)
            *first = temp;
        return newList;
    }

    static void FreeDelegateList(DelegateImplBase* list)
    {
        DelegateImplBase* temp = NULL;
        while (list != NULL)
        {
            temp = list->Previous;
            delete list;
            list = temp;
        }
    }

    static void InvokeDelegateList(DelegateImplBase* list GS_DELEGATE_COMMA GS_DELEGATE_FUNCTION_PARAMS)
    {
        if (list != NULL)
        {
            if (list->Previous != NULL)
                InvokeDelegateList(list->Previous GS_DELEGATE_COMMA GS_DELEGATE_FUNCTION_ARGS);
            list->Invoke(GS_DELEGATE_FUNCTION_ARGS);
        }
    }

// Methods
public:
    template <class TFunctor>
    void Add(const TFunctor& f)
    {
        DelegateImplBase* d = new DelegateImpl<TFunctor>(f);
        d->Previous = this->_last;
        this->_last = d;
    }

	template<class TPtr, class TFunctionPtr>
    void Add(const TPtr& obj, const TFunctionPtr& mfp)
    {
        DelegateImplBase* d = new DelegateImpl<std::pair<TPtr, TFunctionPtr> >(std::make_pair(obj, mfp));
        d->Previous = this->_last;
        this->_last = d;
    }

    template <class TFunctor>
    bool Remove(const TFunctor& f)
    {
        DelegateImplBase* d = this->_last;
        DelegateImplBase** pp = &this->_last;
        DelegateImpl<TFunctor>* impl = NULL;

        while (d != NULL)
        {
            impl = dynamic_cast<DelegateImpl<TFunctor>*>(d);
            if (impl != NULL && impl->Functor == f)
            {
                *pp = d->Previous;
                delete impl;
                return true;
            }
            pp = &d->Previous;
            d = d->Previous;
        }
        return false;
    }

	template<class TPtr, class TFunctionPtr>
    bool Remove(const TPtr& obj, const TFunctionPtr& mfp)
    {
        return Remove(std::make_pair(obj, mfp));
    }

    void Clear()
    {
        FreeDelegateList(this->_last);
        this->_last = NULL;
    }

private:
    template <class TFunctor>
    bool Equals(const TFunctor& f) const
    {
        if (this->_last == NULL || this->_last->Previous != NULL)
            return false;

        DelegateImpl<TFunctor>* impl = 
            dynamic_cast<DelegateImpl<TFunctor>*>(this->_last);
        if (impl == NULL)
            return false;
        return (impl->Functor == f);
    }

// Operators
public:
    operator bool() const
    {
        return !IsEmpty();
    }

    bool operator!() const
    {
        return IsEmpty();
    }

    template <class TFunctor>
    GsDelegate& operator=(const TFunctor& f)
    {
        DelegateImplBase* d = new DelegateImpl<TFunctor>(f);
        FreeDelegateList(this->_last);
        this->_last = d;
        return *this;
    }

    GsDelegate& operator=(const GsDelegate& d)
    {
        if (this != &d)
        {
            DelegateImplBase* list = CloneDelegateList(d._last, NULL);
            FreeDelegateList(this->_last);
            this->_last = list;
        }
        return *this;
    }

    template <class TFunctor>
    GsDelegate& operator+=(const TFunctor& f)
    {
        Add(f);
        return *this;
    } 

    template <class TFunctor>
    GsDelegate& operator-=(const TFunctor& f)
    {
        Remove(f);
        return *this;
    }

    template <class TFunctor>
    GsDelegate operator-(const TFunctor& f) const
    {
        return (GsDelegate(*this) -= f);
    } 
    R operator()(GS_DELEGATE_FUNCTION_PARAMS) const
    {
        if (this->_last != NULL)
		{
			if (this->_last->Previous != NULL)
				InvokeDelegateList(this->_last->Previous GS_DELEGATE_COMMA GS_DELEGATE_FUNCTION_ARGS);
			return this->_last->Invoke(GS_DELEGATE_FUNCTION_ARGS);
		} 
    }
};

//UTILITY_ENDNS

#undef GS_DELEGATE_TEMPLATE_PARAMS
#undef GS_DELEGATE_TEMPLATE_ARGS
#undef GS_DELEGATE_FUNCTION_PARAMS
#undef GS_DELEGATE_FUNCTION_ARGS
#undef GS_DELEGATE_COMMA
#undef GS_DELEGATE_NUM_ARGS