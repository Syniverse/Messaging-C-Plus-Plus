#pragma once

#include <boost/fusion/adapted.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "restc-cpp/logging.h"
#include "scgapi/Scg.h"


namespace scg_api {

template <typename T>
struct ListReturnMapper {
    std::vector<T> list;
    int limit = 0;
    int total = 0;
};

/*! Simple forward iteratable result-set.
 *
 * The interface is design to allow simple for loops over the
 * result-set, including:
 *
 *      for(auto& data : res.List()) {
 *           ...
 *      }
 *
 * \note This is not a fully implemented C++ container-like
 *      class.
 *      Currently, begin() will start at the current offset of the
 *      result-set, so it's result will depend on previous
 *      iterator operations.
 */
template <typename T>
class AsyncForwardList
{
public:
    /*! Forward only iterator for result-sets.
     *
     * The SCG REST server will only return a small number
     * of objects in response to queries. This iterator lets
     * you iterate over the result-set as if all objects was
     * returned by the server. It will asynchronously query the
     * server for more data on demand in the background.
     *
     */
    class AsyncForwardIterator {
    public:
        AsyncForwardIterator(AsyncForwardList& parent, bool end)
        : parent_{parent}, end_{end}
        {
        }

        /*! Get a reference to the current object */
        T& operator * () {
            return parent_.GetCurrent();
        }

        /*! Access the current object as if it was a pointer */
        T* operator -> () {
            return &parent_.GetCurrent();
        }

        /*! Test for end of sequence.
         *
         * \note It will only return true if the current iterator
         * points to the end of the sequence.
         */
        bool operator == (const AsyncForwardIterator& v) const noexcept {
            return Ended() && v.Ended();
        }

        /*! Test for not end of sequence */
        bool operator != (const AsyncForwardIterator& v) const noexcept {
            return !operator == (v);
        }

        /*! Increment the iterator */
        AsyncForwardIterator& operator ++ () { // prefix operator
            parent_.Next();
            return *this;
        }

        /*! Return the number of pages fetched from the server.
         *
         * A page corresponds to one query resuest to the server
         */

        std::size_t GetPagesFetched() const {
            return parent_.GetPagesFetched();
        }

    private:
        bool Ended() const noexcept {
            return end_ || parent_.Ended();
        }

        AsyncForwardList& parent_;
        const bool end_;
    };

    using value_type = T;
    using size_t = std::size_t;
    using iterator = AsyncForwardIterator;
    using list_return_mappert_t = ListReturnMapper<T>;
    using list_t = std::unique_ptr<list_return_mappert_t>;
    using fetch_fn_t = std::function<list_t(int offset)>;

    /*! \internal */
    AsyncForwardList(fetch_fn_t fetch_fn,
                     std::int64_t start_offset = 0)
    : offset_{start_offset}, fetch_fn_{fetch_fn}
    {
    }

    iterator begin() {
        Fetch();
        return AsyncForwardIterator(*this, false);
    }

    iterator end() {
        return AsyncForwardIterator(*this, true);
    }

    /*! \internal */
    T& GetCurrent() {
        assert(HaveCurrent());
        return list_->list[current_];
    }

    /*! \internal */
    void Next() {
        assert(HaveCurrent());
        if (++current_ >= static_cast<decltype(current_)>(list_->list.size())) {
            if (list_ && (static_cast<decltype(current_)>(list_->total)
                <= (offset_ + current_))) {
                reached_end_ = true;
            } else {
                Fetch();
            }
        }
    }

    /*! \internal */
    bool HaveCurrent() const noexcept {
        return !reached_end_ && list_
            && (static_cast<decltype(current_)>(list_->list.size()) > current_);
    }

    /*! \internal */
    bool Ended() const noexcept {
        return reached_end_;
    }

    /*! Return a real C++ container with all the objects
     *
     * \note Use with caution. If your query returns millions of
     *          objects, you probably don't want to use this
     *          method.
     */
    template <typename contT = std::vector<T>>
    contT ToContainer() {
        contT cont;
        for(auto& obj : *this) {
            cont.push_back(std::move(obj));
        }

        return std::move(cont);
    }

    /*! \internal */
    std::size_t GetPagesFetched() const {
        return pages_fected_;
    }

private:
    void Fetch() {
        offset_ += current_;
        current_ = 0;
        list_ = fetch_fn_(offset_);
        if (!HaveCurrent()) {
            reached_end_ = true;
            list_.reset();
        } else {
            ++pages_fected_;

            RESTC_CPP_LOG_DEBUG << "---> Received page with "
                << list_->list.size() << " entries.";
        }
    };

    list_t list_;
    int64_t offset_ = 0;
    std::int64_t current_ = 0;
    bool reached_end_ = false;
    fetch_fn_t fetch_fn_;
    size_t pages_fected_ = 0;
};



} // namespace

BOOST_FUSION_ADAPT_TPL_STRUCT(
    (T),
    (scg_api::ListReturnMapper)(T),
    (typename BOOST_IDENTITY_TYPE((std::vector<T>)), list)
    (int, limit)
    (int, total)
)
