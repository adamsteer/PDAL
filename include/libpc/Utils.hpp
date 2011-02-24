/******************************************************************************
* Copyright (c) 2011, Michael P. Gerlek (mpg@flaxen.com)
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#ifndef INCLUDED_UTILS_HPP
#define INCLUDED_UTILS_HPP

#include <iosfwd>
#include <string>

#include <boost/concept_check.hpp>
#include <boost/cstdint.hpp>

#include "libpc/export.hpp"

namespace libpc
{

class LIBPC_DLL Utils
{
public:
    static void Utils::random_seed(unsigned int seed);
    static double random(double minimum, double maximum);

    // compares two values to within the datatype's epsilon
    template<class T>
    static bool compare_distance(const T& actual, const T& expected)
    {
        const T epsilon = std::numeric_limits<T>::epsilon();
        return compare_approx<T>(actual, expected, epsilon);
    }

    // compares two values to within a given tolerance
    template<class T>
    static bool compare_approx(const T& actual, const T& expected, const T& tolerance)
    {
        const T diff = actual - expected;

        if ( !((diff <= tolerance) && (diff >= -tolerance )) )
        {
            return false;
        }
        return true;
    }

    // open existing file for reading
    static std::istream* openFile(std::string const& filename, bool asBinary=true);

    // open new file for writing
    static std::ostream* createFile(std::string const& filename, bool asBinary=true);

    static void closeFile(std::ostream* ofs);
    static void closeFile(std::istream* ifs);

    static bool deleteFile(const std::string& filename);
    static void renameFile(const std::string& dest, const std::string& src);
    static bool fileExists(const std::string& filename);
    static boost::uintmax_t fileSize(const std::string& filename);

    template<class T>
    static inline void write_field(boost::uint8_t*& dest, T v)
    {
        *(T*)dest = v;
        dest += sizeof(T);
        return;
    }
    
    template<class T>
    static inline T read_field(boost::uint8_t*& src)
    {
        T tmp = *(T*)src;
        src += sizeof(T);
        return tmp;
    }

    template <typename T>
    static inline void read_n(T& dest, std::istream& src, std::streamsize const& num)
    {
        if (!src)
            throw std::runtime_error("detail::liblas::read_n<T> input stream is not readable");

        char* p = as_buffer(dest);
        src.read(p, num);

        // BUG: Fix little-endian
        //LIBLAS_SWAP_BYTES_N(dest, num);

        assert(check_stream_state(src));
    }

    template <typename T>
    static inline void write_n(std::ostream& dest, T const& src, std::streamsize const& num)
    {
        if (!dest)
            throw std::runtime_error("detail::liblas::write_n<T>: output stream is not writable");

        // BUG: Fix little-endian
        T& tmp = const_cast<T&>(src);
        //LIBLAS_SWAP_BYTES_N(tmp, num);

        char const* p = as_bytes(tmp);
        dest.write(p, num);
        assert(check_stream_state(dest));
    }

private:
    template<typename T>
    static inline char* as_buffer(T& data)
    {
        return static_cast<char*>(static_cast<void*>(&data));
    }

    template<typename T>
    static inline char* as_buffer(T* data)
    {
        return static_cast<char*>(static_cast<void*>(data));
    }

    template<typename T>
    static inline char const* as_bytes(T const& data)
    {
        return static_cast<char const*>(static_cast<void const*>(&data));
    }

    template<typename T>
    static inline char const* as_bytes(T const* data)
    {
        return static_cast<char const*>(static_cast<void const*>(data));
    }


    template <typename C, typename T>
    static inline bool check_stream_state(std::basic_ios<C, T>& srtm)
    {
        // Test stream state bits
        if (srtm.eof()) 
            throw std::out_of_range("end of file encountered");
        else if (srtm.fail())
            throw std::runtime_error("non-fatal I/O error occured");
        else if (srtm.bad())
            throw std::runtime_error("fatal I/O error occured");
        return true;
    }
};

} // namespace libpc

#endif
