// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    xively.c
 * \brief   Xively C library [see xively.h]
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "xi_allocator.h"
#include "xively.h"
#include "xi_macros.h"
#include "xi_debug.h"
#include "xi_helpers.h"
#include "xi_err.h"
#include "xi_globals.h"

#include "common.h"
#include "layer_api.h"
#include "layer_interface.h"
#include "layer_connection.h"
#include "layer_types_conf.h"
#include "layer_factory.h"
#include "layer_factory_conf.h"
#include "layer_default_allocators.h"
#include "http_layer.h"
#include "http_layer_data.h"


#include "csv_layer.h"

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------
// HELPER MACROS
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// HELPER FUNCTIONS
//-----------------------------------------------------------------------

xi_value_type_t xi_get_value_type( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    return p->value_type;
}

xi_datapoint_t* xi_set_value_i32( xi_datapoint_t* p, int32_t value )
{
    // PRECONDITION
    assert( p != 0 );

    p->value.i32_value  = value;
    p->value_type       = XI_VALUE_TYPE_I32;

    return p;
}

int32_t xi_get_value_i32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );
    assert( p->value_type == XI_VALUE_TYPE_I32 );

    return p->value.i32_value;
}

int32_t* xi_value_pointer_i32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    if( p->value_type == XI_VALUE_TYPE_I32 )
    {
      return &p->value.i32_value;
    }
    else
    {
      return NULL;
    }
}

xi_datapoint_t* xi_set_value_f32( xi_datapoint_t* p, float value )
{
    // PRECONDITION
    assert( p != 0 );

    p->value.f32_value  = value;
    p->value_type       = XI_VALUE_TYPE_F32;

    return p;
}

float xi_get_value_f32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );
    assert( p->value_type == XI_VALUE_TYPE_F32 );

    if( p->value_type == XI_VALUE_TYPE_F32 )
    {
      return p->value.f32_value;
    }
    else
    {
      return 0.;
    }
}

float* xi_value_pointer_f32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    if( p->value_type == XI_VALUE_TYPE_F32 )
    {
      return &p->value.f32_value;
    }
    else
    {
      return NULL;
    }
}

xi_datapoint_t* xi_set_value_str( xi_datapoint_t* p, const char* value )
{
    // PRECONDITION
    assert( p != 0 );

    int s = xi_str_copy_untiln( p->value.str_value
        , XI_VALUE_STRING_MAX_SIZE, value, '\0' );

    XI_CHECK_SIZE( s, XI_VALUE_STRING_MAX_SIZE, XI_DATAPOINT_VALUE_BUFFER_OVERFLOW );

    p->value_type = XI_VALUE_TYPE_STR;

    return p;

err_handling:
    return 0;
}

char* xi_value_pointer_str( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    if( p->value_type == XI_VALUE_TYPE_STR )
    {
      return p->value.str_value;
    }
    else
    {
      return NULL;
    }
}

void xi_set_network_timeout( uint32_t timeout )
{
    xi_globals.network_timeout = timeout;
}

uint32_t xi_get_network_timeout( void )
{
    return xi_globals.network_timeout;
}

//-----------------------------------------------------------------------
// LAYERS SETTINGS
//-----------------------------------------------------------------------

#define XI_IO_POSIX 0
#define XI_IO_DUMMY 1
#define XI_IO_MBED  2

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief The LAYERS_ID enum
///
enum LAYERS_ID
{
      IO_LAYER = 0
    , HTTP_LAYER
    , CSV_LAYER
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CONNECTION_SCHEME_1_DATA IO_LAYER, HTTP_LAYER, CSV_LAYER
DEFINE_CONNECTION_SCHEME( CONNECTION_SCHEME_1, CONNECTION_SCHEME_1_DATA );

#if XI_COMM_LAYER == XI_IO_POSIX

    // posix io layer
    #include "posix_io_layer.h"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_LAYER_TYPES_CONF()
          LAYER_TYPE( IO_LAYER, &posix_io_layer_data_ready, &posix_io_layer_on_data_ready
                              , &posix_io_layer_close, &posix_io_layer_on_close )
        , LAYER_TYPE( HTTP_LAYER, &http_layer_data_ready, &http_layer_on_data_ready
                                , &http_layer_close, &http_layer_on_close )
        , LAYER_TYPE( CSV_LAYER, &csv_layer_data_ready, &csv_layer_on_data_ready
                            , &csv_layer_close, &csv_layer_on_close )
    END_LAYER_TYPES_CONF()

#elif XI_COMM_LAYER == XI_IO_DUMMY
    // dummy io layer
    #include "dummy_io_layer.h"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_LAYER_TYPES_CONF()
          LAYER_TYPE( IO_LAYER, &dummy_io_layer_data_ready, &dummy_io_layer_on_data_ready
                              , &dummy_io_layer_close, &dummy_io_layer_on_close )
        , LAYER_TYPE( HTTP_LAYER, &http_layer_data_ready, &http_layer_on_data_ready
                                , &http_layer_close, &http_layer_on_close )
        , LAYER_TYPE( CSV_LAYER, &csv_layer_data_ready, &csv_layer_on_data_ready
                            , &csv_layer_close, &csv_layer_on_close )
    END_LAYER_TYPES_CONF()

#elif XI_COMM_LAYER == XI_IO_MBED
    // mbed io layer
    #include "mbed_io_layer.h"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_LAYER_TYPES_CONF()
          LAYER_TYPE( IO_LAYER, &mbed_io_layer_data_ready, &mbed_io_layer_on_data_ready
                              , &mbed_io_layer_close, &mbed_io_layer_on_close )
        , LAYER_TYPE( HTTP_LAYER, &http_layer_data_ready, &http_layer_on_data_ready
                                , &http_layer_close, &http_layer_on_close )
        , LAYER_TYPE( CSV_LAYER, &csv_layer_data_ready, &csv_layer_on_data_ready
                            , &csv_layer_close, &csv_layer_on_close )
    END_LAYER_TYPES_CONF()

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_FACTORY_CONF()
      FACTORY_ENTRY( IO_LAYER, &placement_layer_pass_create, &placement_layer_pass_delete
                             , &default_layer_stack_alloc, &default_layer_stack_free )
    , FACTORY_ENTRY( HTTP_LAYER, &placement_layer_pass_create, &placement_layer_pass_delete
                               , &default_layer_stack_alloc, &default_layer_stack_free )
    , FACTORY_ENTRY( CSV_LAYER, &placement_layer_pass_create, &placement_layer_pass_delete
                           , &default_layer_stack_alloc, &default_layer_stack_free )
END_FACTORY_CONF()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------
// MAIN LIBRARY FUNCTIONS
//-----------------------------------------------------------------------

xi_context_t* xi_create_context(
      xi_protocol_t protocol, const char* api_key
    , xi_feed_id_t feed_id )
{
    // allocate the structure to store new context
    xi_context_t* ret = ( xi_context_t* ) xi_alloc( sizeof( xi_context_t ) );

    XI_CHECK_MEMORY( ret );

    // copy given numeric parameters as is
    ret->protocol       = protocol;
    ret->feed_id        = feed_id;

    // copy string parameters carefully
    if( api_key )
    {
        // duplicate the string
        ret->api_key  = xi_str_dup( api_key );

        XI_CHECK_MEMORY( ret->api_key );
    }
    else
    {
        ret->api_key  = 0;
    }

    switch( protocol )
    {
        case XI_HTTP:
            {
                // @TODO make a configurable pool of these
                // static structures allocated statically
                static http_layer_data_t    http_layer_data;
                static csv_layer_data_t     csv_layer_data;
                static xi_response_t        xi_response;


                // clean the structures
                memset( &http_layer_data, 0, sizeof( http_layer_data_t ) );
                memset( &csv_layer_data, 0, sizeof( csv_layer_data_t ) );
                memset( &xi_response, 0, sizeof( xi_response_t ) );

                // the response pointer
                http_layer_data.response    = &xi_response;
                csv_layer_data.response     = &xi_response;

                // prepare user data description
                void* user_datas[] = { 0, ( void* ) &http_layer_data, ( void* ) &csv_layer_data };

                // create and connect layers store the information in layer_chain member
                ret->layer_chain = create_and_connect_layers( CONNECTION_SCHEME_1, user_datas, CONNECTION_SCHEME_LENGTH( CONNECTION_SCHEME_1 ) );
            }
            break;
        default:
            goto err_handling;
    }


    return ret;

err_handling:
    if( ret )
    {
        if( ret->api_key )
        {
            XI_SAFE_FREE( ret->api_key );
        }

        XI_SAFE_FREE( ret );
    }

    return 0;
}

void xi_delete_context( xi_context_t* context )
{
    if( context )
    {
        XI_SAFE_FREE( context->api_key );
    }
    XI_SAFE_FREE( context );
}

const xi_response_t* xi_feed_get(
          xi_context_t* xi
        , xi_feed_t* feed )
{
    layer_t* io_layer = connect_to_endpoint( xi->layer_chain.bottom, XI_HOST, XI_PORT );

    if( io_layer == 0 )
    {
        // we are in trouble
        return 0;
    }

    // extract the input layer
    layer_t* input_layer = xi->layer_chain.top;

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_FEED_GET
        , xi
        , 0
        , { .xi_get_feed = { .feed = feed } }
    };

    CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );
    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

const xi_response_t* xi_feed_update(
          xi_context_t* xi
        , const xi_feed_t* feed )
{
    layer_t* io_layer = connect_to_endpoint( xi->layer_chain.bottom, XI_HOST, XI_PORT );

    if( io_layer == 0 )
    {
        // we are in trouble
        return 0;
    }

    // extract the input layer
    layer_t* input_layer = xi->layer_chain.top;

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_FEED_UPDATE
        , xi
        , 0
        , { .xi_update_feed = { ( xi_feed_t * ) feed } }
    };

    CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );
    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

const xi_response_t* xi_datastream_get(
            xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id, xi_datapoint_t* o )
{
    XI_UNUSED( feed_id );

    layer_t* io_layer = connect_to_endpoint( xi->layer_chain.bottom, XI_HOST, XI_PORT );

    if( io_layer == 0 )
    {
        // we are in trouble
        return 0;
    }

    // extract the input layer
    layer_t* input_layer = xi->layer_chain.top;

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATASTREAM_GET
        , xi
        , 0
        , { ( struct xi_get_datastream_t ) { datastream_id, o } }
    };

    CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );
    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}


const xi_response_t* xi_datastream_create(
            xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id
          , const xi_datapoint_t* datapoint )
{
    XI_UNUSED( feed_id );

    layer_t* io_layer = connect_to_endpoint( xi->layer_chain.bottom, XI_HOST, XI_PORT );

    if( io_layer == 0 )
    {
        // we are in trouble
        return 0;
    }

    // extract the input layer
    layer_t* input_layer = xi->layer_chain.top;

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATASTREAM_CREATE
        , xi
        , 0
        , { .xi_create_datastream = { ( char* ) datastream_id, ( xi_datapoint_t* ) datapoint } }
    };

    CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );
    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

const xi_response_t* xi_datastream_update(
          xi_context_t* xi, xi_feed_id_t feed_id
        , const char * datastream_id
        , const xi_datapoint_t* datapoint )
{
    XI_UNUSED( feed_id );

    layer_t* io_layer = connect_to_endpoint( xi->layer_chain.bottom, XI_HOST, XI_PORT );

    if( io_layer == 0 )
    {
        // we are in trouble
        return 0;
    }

    // extract the input layer
    layer_t* input_layer = xi->layer_chain.top;

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATASTREAM_UPDATE
        , xi
        , 0
        , { .xi_update_datastream = { ( char* ) datastream_id, ( xi_datapoint_t* ) datapoint } }
    };

    CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );
    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

const xi_response_t* xi_datastream_delete(
            xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id )
{
    XI_UNUSED( feed_id );

    layer_t* io_layer = connect_to_endpoint( xi->layer_chain.bottom, XI_HOST, XI_PORT );

    if( io_layer == 0 )
    {
        // we are in trouble
        return 0;
    }

    // extract the input layer
    layer_t* input_layer = xi->layer_chain.top;

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATASTREAM_DELETE
        , xi
        , 0
        , { .xi_delete_datastream = { datastream_id } }
    };

    CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );
    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

const xi_response_t* xi_datapoint_delete(
          const xi_context_t* xi, xi_feed_id_t feed_id
        , const char * datastream_id
        , const xi_datapoint_t* o )
{
    XI_UNUSED( feed_id );

    layer_t* io_layer = connect_to_endpoint( xi->layer_chain.bottom, XI_HOST, XI_PORT );

    if( io_layer == 0 )
    {
        // we are in trouble
        return 0;
    }

    // extract the input layer
    layer_t* input_layer = xi->layer_chain.top;

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATAPOINT_DELETE
        , ( xi_context_t* ) xi
        , 0
        , { .xi_delete_datapoint = { ( char* ) datastream_id, ( xi_datapoint_t* ) o } }
    };

    CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );
    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

extern const xi_response_t* xi_datapoint_delete_range(
            const xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id
          , const xi_timestamp_t* start
          , const xi_timestamp_t* end )
{
    XI_UNUSED( feed_id );

    layer_t* io_layer = connect_to_endpoint( xi->layer_chain.bottom, XI_HOST, XI_PORT );

    if( io_layer == 0 )
    {
        // we are in trouble
        return 0;
    }

    // extract the input layer
    layer_t* input_layer = xi->layer_chain.top;

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATAPOINT_DELETE_RANGE
        , ( xi_context_t* ) xi
        , 0
        , { .xi_delete_datapoint_range = { ( char* ) datastream_id, ( xi_timestamp_t* ) start, ( xi_timestamp_t* ) end } }
    };

    CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );
    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}


#ifdef __cplusplus
}
#endif
