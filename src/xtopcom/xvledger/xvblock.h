// Copyright (c) 2018-2020 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xdata.h"
#include "xbase/xmem.h"
#include "xbase/xobject_ptr.h"
#include "xventity.h"
#include "xvtransact.h"
#include "xvblock_fork.h"
#include "xvledger/xvblock_extra.h"
#include "xstatistic/xstatistic.h"

namespace top
{
    namespace base
    {
        class xvbindex_t;
        class xvbstate_t;

        /*  Very High Level structre/View

         #1: The xvblock manage unified block struture, v-header connected as chain, v-cert as proof for v-header ,and v-body carry business(txs,cmds etc).
             The binary data of full-block in DB has following layout,xvblock manage {Head+Body} directly,and link to xvqcert that manage {certification}

                          ____________           ____________
                         |   v-Head   | <------ |   v-Head   |
                         |------------|         |------------|
                         |   v-Body   |         |   v-Body   |
                         |------------|         |------------|
                         |   v-cert   |         |   v-cert   |
                          ------------           ------------

         #2:  The goal of v-cert to manage unified qc-cert or proof that keep consisten under one Round/View of consensus,and isolate detail from muti-sign and v-header
              The binary data of v-cert in DB has following layout. xvqcert_t manage and link to  multi-sign object who BLS or Schnorr


                          ____________
                         |  view_id   |
                         |------------|
                         | last-view# |
                         |------------|
                         | view-leader|
                         |------------|
                         |   weight   |
                         |------------|
                         | view_stage |
                         |------------|
                         |  mult-sign |
                          ------------

         #3: The binary data of muti-signature like below,that changed by every sign of replica. here not explore detail how crytpo module usse different objects to support BLS or Schnorr.
                          ____________
                         |    type    |
                         |------------|
                         |   random   |
                         |------------|
                         |  ids-bits  |
                         |------------|
                         |  signature |
                          ------------
         */


        /////////////////////////2 byte for vblock types /////////////////////////
        //[1][enum_xvblock_level][enum_xvblock_class][enum_xvblock_type][enum_xvblock_character] =  [1][3][3][7][2] = 16bits

        //total 3bits = max 8 definition
        enum enum_xvblock_level
        {
            enum_xvblock_level_unit     = 0, //account block
            enum_xvblock_level_table    = 1, //table   block
            enum_xvblock_level_book     = 2, //book    block
            enum_xvblock_level_zone     = 3, //top  level of a zone
            enum_xvblock_level_chain    = 4, //top  level of inside one single chain
            enum_xvblock_level_root     = 5, //cross every chain(include main-chain, service chains).e.g global clock block
        };

        //total 3bits = max 8 definition = [2:category][4 sub type]
        enum enum_xvblock_class
        {
            enum_xvblock_class_invalid  = 0, //invalid
            enum_xvblock_class_nil      = 1, //empty block dont have any commands/txs,
            enum_xvblock_class_light    = 2, //ligth block with incremental change based on full_block
            enum_xvblock_class_full     = 3, //full  block with final state by combining every thing
        };

        //total 7 bit = max 127 definition for biz type
        enum enum_xvblock_type
        {
            enum_xvblock_type_general   = 0, //untyped
            enum_xvblock_type_genesis   = 1, //first block of every chain,with block height of 0,viewid of 0 and clock of 0            
            // enum_xvblock_type_boot      = 2, //bootstrap block
            enum_xvblock_type_fullunit  = 2, // fullunit with class_nil
            enum_xvblock_type_clock     = 3, //clock related block
            enum_xvblock_type_lightunit = 4, // lightunit with class_nil
            // enum_xvblock_type_node      = 4, //node related  block,e.g. node join/leave
            // enum_xvblock_type_user      = 5, //user related  block,e.g. create account/destroy account
            enum_xvblock_type_txs       = 6, //transaction(financial/token) related
            // enum_xvblock_type_cmds      = 7, //smart-contract related
            // enum_xvblock_type_view      = 8, //view block or timeout block
            // enum_xvblock_type_election  = 9, //election block from REC/ZEC etc
            enum_xvblock_type_batch     = 10,//that is a batch group(like table or book)
        };

        //three body type
        enum enum_xvblock_body_type
        {
            enum_xvblock_body_type_input            = 0x0001,
            enum_xvblock_body_type_output           = 0x0002,
            enum_xvblock_body_type_outputoffdata    = 0x0004,
            enum_xvblock_body_type_none             = 0,
            enum_xvblock_body_type_input_output     = enum_xvblock_body_type_input|enum_xvblock_body_type_output,
            enum_xvblock_body_type_output_outputoffdata = enum_xvblock_body_type_output|enum_xvblock_body_type_outputoffdata,
            enum_xvblock_body_type_all              = enum_xvblock_body_type_input|enum_xvblock_body_type_output|enum_xvblock_body_type_outputoffdata,
        };

        //total 2bits = max 4 definition
        enum enum_xvblock_character // for future definition
        {
            //once enum_xvblock_character_certify_header_only enable it ,which ask:
                //ask xvqcert.m_header_hash = hash(xvheader)
                //xvheader.input_hash/output_hash must be valid
            //but it disabled as default,which mean xvqcert.m_header_hash = hash(xvheader+xvinput+xvoutput)
            enum_xvblock_character_certify_header_only = 0x01,
            //xvblock only include xvheader,just for unit block
            enum_xvblock_character_simple_unit         = 0x02,
        };

        constexpr uint64_t TOP_BEGIN_GMTIME = 1573189200;//1573189200 == 2019-11-08 05:00:00  UTC

        uint64_t clock_to_gmtime(uint64_t clock);

        //The definition of version defintion for binary of node and block:[8:Features][8:MAJOR][8:MINOR][8:PATCH]
            //Features: added new featurs that need most node agree then make it effect,refer BIP8 spec
            //MAJOR： version when make incompatible API changes
            //MINOR： version when add  functionality in a backwards compatible manner
            //PATCH： version when make backwards compatible bug fixes.
        //virual header of virtual-block
        //note:once xvheader_t is created or serialized_from,not allow change it'data anymore
        class xvheader_t : public xobject_t, public xstatistic::xstatistic_obj_face_t
        {
            friend class xvblock_t;
            friend class xvblockstore_t;
            friend class xvblockbuild_t;
        public:
            static  const std::string   name(){ return std::string("xvheader");}
            virtual std::string         get_obj_name() const override {return name();}
            enum{enum_obj_type = enum_xobject_type_vheader};//allow xbase create xvheader_t object from xdataobj_t::read_from()

            static uint32_t             make_block_version(uint8_t major,uint8_t minor,uint8_t patch){ return ( (((uint32_t)major) << 16) | (((uint32_t)minor) << 8) | ((uint32_t)patch) );}

            static uint32_t             cal_block_features(const uint32_t version)     { return (version >> 24); }
            static int                  cal_block_version_major(const uint32_t version){ return ((version& 0x00FF0000) >> 16);}
            static int                  cal_block_version_minor(const uint32_t version){ return ((version& 0x0000FF00) >> 8);}
            static int                  cal_block_version_patch(const uint32_t version){ return ((version& 0x000000FF));}

            //[0][enum_xvblock_level][enum_xvblock_class][enum_xvblock_type][enum_xvblock_character] =  [1][3][3][7][2] = 16bits
            static enum_xvblock_level   cal_block_level(const int types){return (enum_xvblock_level)((types >> 12) & 0x07);}
            static enum_xvblock_class   cal_block_class(const int types){return (enum_xvblock_class)((types >> 9) & 0x07);}
            static enum_xvblock_type    cal_block_type(const int types){return (enum_xvblock_type) ((types >> 2) & 0x7F);}
            
            static int                  cal_block_character(const int types){ return (types & 0x03);};
        public:
            xvheader_t(const xvheader_t & other);

        protected:
            xvheader_t();
            virtual ~xvheader_t();
            xvheader_t & operator = (const xvheader_t & other);

        public:
            virtual bool               is_valid()  const;
            virtual bool               is_equal(const xvheader_t & other) const;
            virtual void*              query_interface(const int32_t _enum_xobject_type_) override final;//caller need to cast (void*) to related ptr
            virtual xauto_ptr<xvheader_t> clone() const;
            virtual std::string        dump() const override;  //just for debug purpose
        public:
            //[0][enum_xvblock_level][enum_xvblock_class][enum_xvblock_type][enum_xvblock_state_mode] =  [1][3][3][7][2] = 16bits
            inline enum_xvblock_level          get_block_level()  const {return (enum_xvblock_level)((m_types >> 12) & 0x07);}
            inline enum_xvblock_class          get_block_class()  const {return (enum_xvblock_class)((m_types >> 9) & 0x07);}
            inline enum_xvblock_type           get_block_type()   const {return (enum_xvblock_type) ((m_types >> 2) & 0x7F);}
            inline int                         get_block_characters()const {return (m_types & 0x03);}
            inline bool                        is_character_cert_header_only() const {return get_block_characters() & enum_xvblock_character_certify_header_only;}
            inline bool                        is_character_simple_unit() const {return get_block_characters() & enum_xvblock_character_simple_unit;}            
            inline const uint16_t              get_block_raw_types() const {return m_types;}

            //common information for this block
            inline uint32_t                    get_block_features()      const  { return (m_versions >> 24); }
            inline uint32_t                    get_block_version()       const  { return (m_versions & 0x00FFFFFF);}
            inline int                         get_block_version_major() const  { return ((m_versions& 0x00FF0000) >> 16);}
            inline int                         get_block_version_minor() const  { return ((m_versions& 0x0000FF00) >> 8);}
            inline int                         get_block_version_patch() const  { return ((m_versions& 0x000000FF));}
            inline const uint32_t              get_block_raw_versions()  const   {return m_versions;}

            inline uint32_t                    get_chainid()    const {return m_chainid;}//chain id(24bit)
            inline uint64_t                    get_height()     const {return m_height;}
            inline uint64_t                    get_weight()     const {return m_weight;}
            inline const std::string &         get_account()    const {return m_account;}
            inline const std::string &         get_comments()   const {return m_comments;}
            inline const std::string&          get_input_hash() const {return m_input_hash;}
            inline const std::string&          get_output_hash()const {return m_output_hash;}

            //part#2: information about previouse blocks
            inline const std::string &         get_last_block_hash()        const {return m_last_block_hash;}
            inline const std::string &         get_last_full_block_hash()   const {return m_last_full_block_hash;}
            inline const uint64_t              get_last_full_block_height() const {return m_last_full_block_height;}
            //part#3:extra data
            inline const std::string &         get_extra_data()             const {return m_extra_data;}

        protected: //the reason provide beblow setting, just given subclass the flex for construction function

            //[1][enum_xvblock_level][enum_xvblock_class][enum_xvblock_type][enum_xvblock_character] =  [1][3][3][7][2] = 16bits
            inline void                 set_block_level(enum_xvblock_level _level)  {m_types = ( (m_types & 0x8FFF) | (_level << 12));}
            inline void                 set_block_class(enum_xvblock_class _class)  {m_types = ( (m_types & 0xF1FF) | (_class << 9 ));}
            inline void                 set_block_type(enum_xvblock_type  _type)    {m_types = ( (m_types & 0xFE03) | (_type  << 2));}

            inline void                 set_block_character(enum_xvblock_character it){m_types |= (it & 0x03);}
            inline void                 set_block_features(const uint32_t functions_list){m_versions = ((m_versions & 0x00FFFFFF) | (functions_list << 24));}
            inline void                 set_block_version(const uint32_t version)   {m_versions = ((m_versions & 0xFF000000) | (version & 0x00FFFFFF));}

            //inline void                 set_block_reserved(enum_xvblock_reserved val)   {m_types = ( (m_types & 0xFFFC) | (val   ));}
            inline void                 set_chainid(const uint32_t chainid)         {m_chainid = chainid;}
            inline void                 set_height(const uint64_t height)           {m_height = height;}
            inline void                 set_weight(const uint64_t weight)           {m_weight = weight;}
            inline void                 set_account(const std::string & account)    {m_account = account;}
            inline void                 set_comments(const std::string & comments)  {m_comments= comments;}

            inline void                 set_last_block_hash(const std::string & hash){m_last_block_hash = hash;}
            inline void                 set_last_full_block(const std::string & hash,const uint64_t height) {m_last_full_block_hash = hash;m_last_full_block_height = height;}

            inline void                 set_extra_data(const std::string& extradata){m_extra_data = extradata;}
            
        public:
            int32_t             serialize_to(xstream_t & stream); //serialize header and object,return how many bytes is writed
            //just wrap function for serialize_to(),but assign data to string and return
            int32_t             serialize_to_string(std::string & bin_data);

            virtual int32_t    get_class_type() const override {return xstatistic::enum_statistic_block_header;}
        private:
            //return how many bytes readout /writed in, return < 0(enum_xerror_code_type) when have error
            int32_t             do_write(xstream_t & stream); //not allow subclass change behavior
            int32_t             do_read(xstream_t & stream); //not allow subclass change behavior

            int32_t             serialize_from(xstream_t & stream);//not allow subclass change behavior
            
            //just open for xvblock_t object to access
            inline void                 set_input_hash(const std::string& input_hash)   {m_input_hash = input_hash;}
            inline void                 set_output_hash(const std::string& output_hash) {m_output_hash = output_hash;}
            
            size_t get_object_size_real() const override;
        private:
            //information about this block
            uint16_t            m_types;        //[1][enum_xvblock_class][enum_xvblock_level][enum_xvblock_type][enum_xvblock_reserved]
            uint32_t            m_versions;     //version of chain as [8:features][8:major][8:minor][8:patch]
            uint32_t            m_chainid;      //uniqie chain id(24bit) globally where m_account stay,it is actually "NetworkID" refer xbase.h
            uint64_t            m_height;       //height for this block
            uint64_t            m_weight;       //accumulative weight of the PoW or PoS until this block
            std::string         m_account;      //block is generated under specific account
            std::string         m_comments;     //reserved to put comments,may fill any data you want
            std::string         m_input_hash;   //just hash(binary data of input),reserved for future
            std::string         m_output_hash;  //just hash(binary data of output),reserved for future

            //note: Block Hash = hash(xvqcert_t)
            //information about previouse blocks
            std::string         m_last_block_hash;       //block hash of parent block,it must have valid
            std::string         m_last_full_block_hash;  //any block need carry the m_last_full_block_hash
            uint64_t            m_last_full_block_height;//height of m_last_full_block
        private:
            std::string         m_extra_data;    //reserved for future
        };

        /////////////////////////1 byte for consenus types/////////////////////////
        //[enum_xconsensus_type][enum_xconsensus_threshold][enum_xconsensus_flag] = [3][2][3] = 8bits
        //3bits = max 8 definition
        enum enum_xconsensus_type
        {
            enum_xconsensus_type_none           = 0,
            enum_xconsensus_type_genesis        = 1,//follow rules of genesis
            enum_xconsensus_type_xhbft          = 2,//xBFT-chain mode
            enum_xconsensus_type_xpow           = 3,//xPoW
            enum_xconsensus_type_xpos           = 4,//xPoS
        };
        //2bit = max 4 definition
        enum enum_xconsensus_threshold
        {
            enum_xconsensus_threshold_anyone    = 0, //just need Leader/Maker,usally for PoW
            enum_xconsensus_threshold_2_of_3    = 1, // 2/3 voted for BFT,as default
            enum_xconsensus_threshold_3_of_4    = 2, // 3/4 voted for BFT
            enum_xconsensus_threshold_reserved  = 3, //undefined yet
        };
        //3bit = max 3 flags  max 8 definition
        enum enum_xconsensus_flag
        {
            //any cert with unspecified flag means just a "prepared-cert" that ask the next 3/2 more certs to proof it
            enum_xconsensus_flag_validator_cert      = 0x00, //validator signature required
            enum_xconsensus_flag_audit_cert          = 0x01, //validator and audit signature required
            enum_xconsensus_flag_extend_vote         = 0x02, //validator signature and extend signature together for relay-table
            enum_xconsensus_flag_simple_cert         = 0x03, //no signature for simple unit mode
            enum_xconsensus_flag_extend_cert         = 0x04, //relyon m_extend_cert to proof
            enum_xconsensus_flag_extend_and_audit_cert = 0x05, // for old version unit,may extend cert and audit cert
        };

        /////////////////////////1 byte for algorithm types /////////////////////////
        //[enum_xvchain_key_curve][enum_xvchain_sign_scheme][enum_xhash_type] = [2][3][3] = 8bits
        //2bit = max 4 definition
        enum enum_xvchain_key_curve
        {
            enum_xvchain_key_curve_secp256k1    = 1,   //secp256k1 curve       ---> Ecdsa signature
            enum_xvchain_key_curve_ed25519      = 2,   //Edward 25519 curve    ---> Eddsa signature
        };
        //3bit = max 8 definition
        enum enum_xvchain_sign_scheme
        {
            enum_xvchain_threshold_sign_scheme_none     = 0, //for genesis block
            enum_xvchain_threshold_sign_scheme_schnorr  = 1, //threshold signature of Schnorr(ECSS)
            enum_xvchain_threshold_sign_scheme_bls      = 2, //threshold signature of BLS
            enum_xvchain_threshold_sign_scheme_ed25519  = 3, //threshold signature for Ed25519

            enum_xvchain_single_sign_scheme_ecdsa       = 4, //based secp256k1
            enum_xvchain_single_sign_scheme_eddsa       = 5, //based ed25519

            enum_xvchain_sign_scheme_max                = 7, //all definition must <= 7
        };
        //3bit = max 8 definition for enum_xhash_type

        //xCertificate like CA Cetification that support hierarchy sign and verify
        class xvqcert_t : public xdataunit_t, public xstatistic::xstatistic_obj_face_t
        {
            friend class xvblock_t;
            friend class xvblockstore_t;
            friend class xvblockbuild_t;
        public:
            static  const std::string   name(){ return std::string("xvqcert");}
            virtual std::string         get_obj_name() const override {return name();}
            enum{enum_obj_type = enum_xobject_type_vqccert};//allow xbase create xvqcert_t object from xdataunit_t::read_from()

        public:
            xvqcert_t(const xvqcert_t & other,enum_xdata_type type = (enum_xdata_type)enum_xobject_type_vqccert);
        protected:
            virtual ~xvqcert_t();
        private:
            xvqcert_t();
            xvqcert_t & operator = (const xvqcert_t & other);
        public:
            virtual bool               is_valid()   const;//test without considering m_verify_cert and m_audit_cert
            virtual bool               is_deliver() const; //test everyting has valid value
            virtual bool               is_equal(const xvqcert_t & other)   const; //test logically without m_verify_cert/audit_cert
            virtual bool               is_same(const xvqcert_t & other)    const; //test everyting as same value
            virtual const std::string  hash(const std::string & input)     const;  //hash by the hash_type of qcert
            //test by comparing the core_network_id of leader 'm_validator or m_auditor,subclass may override if need
            virtual bool               is_validator(const uint64_t replica_xip)  const;
            virtual bool               is_validator(const xvip2_t& replica_xip2) const;
            virtual bool               is_auditor(const uint64_t replica_xip)    const;
            virtual bool               is_auditor(const xvip2_t& replica_xip2)   const;

            virtual const uint32_t     get_validator_threshold()            const;
            virtual const uint32_t     get_auditor_threshold()              const;
            virtual void*              query_interface(const int32_t _enum_xobject_type_) override;//caller need to cast (void*) to related ptr

            virtual std::string        dump() const override;  //just for debug purpose
            virtual xauto_ptr<xvqcert_t> clone() const;
        public:
            inline  uint64_t           get_clock()     const {return m_clock;} //start/create time of this cert
            inline  uint64_t           get_expired()   const {return (m_expired == (uint32_t)-1) ? ((uint64_t)-1) : (m_clock + m_expired);}//return absolute clock'height
            inline  uint64_t           get_gmtime()    const {return ((uint64_t)(m_clock * 10) + TOP_BEGIN_GMTIME);}//return standard gmt times
            inline  uint64_t           get_drand_height() const {return  m_drand_height;}

            inline uint64_t            get_nonce()     const {return m_nonce;}
            inline  uint64_t           get_viewid()    const {return m_viewid;}
            inline  uint32_t           get_viewtoken() const {return m_view_token;}
            inline  const xvip2_t&     get_validator() const {return m_validator;} //XIP address about who issue/lead this block
            inline  const xvip2_t&     get_auditor()   const {return m_auditor;} //XIP address(wild* address) about who(a cluster or a subset) should audit this certication
            inline  uint64_t           get_parent_block_height() const {return m_parent_height;} //return 0 if not existing
            inline  uint64_t           get_parent_block_viewid() const {return m_parent_viewid;} //return 0 if not existing
            
            inline  const std::string& get_header_hash()       const {return m_header_hash;}
            inline  const std::string& get_input_root_hash()   const {return m_input_root_hash;}
            inline  const std::string& get_output_root_hash()  const {return m_output_root_hash;}
            inline  const std::string& get_justify_cert_hash() const {return m_justify_cert_hash;}
            const std::string          get_hash_to_sign()      const;//signatrure' hash = hash(m_header_hash + m_input_hash + m_output_root_hash + m_consensus + m_cryptos..)
            const std::string          build_block_hash();   //recalculate the hash of block, note: block_hash is equal as cert'hash

            inline  const std::string& get_verify_signature()  const {return m_verify_signature;}
            inline  const std::string& get_audit_signature()   const {return m_audit_signature;}
            inline  const std::string& get_extend_data()       const {return m_extend_data;}
            inline  const std::string& get_extend_cert()       const {return m_extend_cert;}

            //[3bit[2bit][3bit] = [enum_xconsensus_type][enum_xconsensus_threshold][enum_xconsensus_audit_flag]
            inline  enum_xconsensus_type       get_consensus_type()      const {return (enum_xconsensus_type)(m_consensus >> 5);}
            inline  enum_xconsensus_threshold  get_consensus_threshold() const {return (enum_xconsensus_threshold)((m_consensus >> 3) & 0x03);}
            inline  int                        get_consensus_flags()     const {return (m_consensus & 0x07);} //lowest 3bit
            bool                               is_consensus_flag_has_extend_cert() const {return get_consensus_flags() == enum_xconsensus_flag_extend_cert || get_consensus_flags() == enum_xconsensus_flag_extend_and_audit_cert;}
            //[enum_xvchain_key_curve][enum_xvchain_sign_scheme][enum_xhash_type] = [2][3][3] = 8bits
            inline  enum_xvchain_key_curve     get_crypto_key_type()     const {return (enum_xvchain_key_curve)(m_cryptos >> 6);}
            inline  enum_xvchain_sign_scheme   get_crypto_sign_type()    const {return (enum_xvchain_sign_scheme)((m_cryptos >> 3) & 0x07);}
            inline  enum_xhash_type            get_crypto_hash_type()    const {return (enum_xhash_type)(m_cryptos & 0x07);}

            inline   uint32_t                  get_modified_count()const {return m_modified_count;}
            uint32_t                           add_modified_count();

        public: //note: not allow do any modify once m_verify_signature/m_audit_signature is valid or  m_parent_cert is valid
            bool                 is_allow_modify() const;  //whether allow to change some content before has valid signature
            void                 reset_block_flags(); //clean all flags related block

            void                 set_drand(const uint64_t global_drand_height);
            void                 set_clock(const  uint64_t  global_clock_round);
            void                 set_expired(const  uint64_t  expired);
            void                 set_nonce(const  uint64_t nonce);
            void                 set_viewid(const uint64_t viewid);
            void                 set_viewtoken(const uint32_t viewtoken);
            void                 set_validator(const xvip2_t & validator_xip);
            void                 set_auditor(const xvip2_t & auditor_xip);
            void                 set_justify_cert_hash(const std::string & hash);
            void                 set_parent_height(const uint64_t parent_block_height);//link to parent container(like tableblock)
            void                 set_parent_viewid(const uint64_t parent_block_viewid);//link to parent container(like tableblock)

            virtual int32_t      serialize_to_string(std::string & bin_data) override final; //wrap function fo serialize_to(stream)

        protected: //note: not allow do any modify once m_verify_signature/m_audit_signature is valid or  m_parent_cert is valid

            //[3bit[2bit][3bit] = [enum_xconsensus_type][enum_xconsensus_threshold][enum_xconsensus_audit_flag]
            void                 set_consensus_type(enum_xconsensus_type type);
            void                 set_consensus_threshold(enum_xconsensus_threshold type);
            void                 set_consensus_flag(enum_xconsensus_flag flag); //lowest 3bit

            //[enum_xvblock_level][enum_xvblock_class][enum_xvblock_type][enum_xvblock_reserved] =  [4][3][7][2] = 16bits
            void                 set_crypto_key_type(enum_xvchain_key_curve type);
            void                 set_crypto_sign_type(enum_xvchain_sign_scheme type);
            void                 set_crypto_hash_type(enum_xhash_type type);

            void                 set_input_root_hash(const std::string & merkle_hash);
            void                 set_output_root_hash(const std::string & merkle_hash);
            bool                 set_header_hash(const std::string& header_binary_data);//return false if not match the existing hash
        private:
            //return how many bytes readout /writed in, return < 0(enum_xerror_code_type) when have error
            virtual int32_t     do_write(xstream_t & stream) override final; //not allow subclass change behavior
            virtual int32_t     do_read(xstream_t & stream)  override final; //not allow subclass change behavior
            virtual int32_t     serialize_to(xstream_t & stream) override final;  //serialize header and object,return how many bytes is writed
            virtual int32_t     serialize_from(xstream_t & stream) override final;//not allow subclass change behavior
            virtual int32_t     serialize_from_string(const std::string & bin_data)override final; //wrap function fo serialize_from(stream)
            void                reset_modified_count();
            
        public: //temporary open for access by outside
            //only open for xvblock_t object to set them after verify singature by CA(xvcertauth_t)
            void                set_verify_signature(const std::string & proof);
            void                set_audit_signature(const std::string & proof);

            void                set_extend_data(const std::string& extention);
            void                set_extend_cert(const std::string & _cert_bin);

            virtual int32_t get_class_type() const override {return xstatistic::enum_statistic_vqcert;}
        private:
            size_t get_object_size_real() const override;

        private: //m_modified_count not serialized into binary,put here make alignment of this class better
            uint32_t            m_modified_count;   //count how many times modified since last save,it is 0 if nothing changed
        private:
            uint64_t            m_nonce;            //each block has random nonce
            uint64_t            m_clock;            //the effective time of this cert from clock cert, note: m_clock is round of global-clock and be 10-seconds since 2019-11-08 05:00:00 UTC(2019-11-08 13:00:00 UTC+8)
            uint32_t            m_expired;          //expired clock'count since m_clock,-1 means valid forever
            uint32_t            m_view_token;       //paried with view#id,random generated token for each view#id and each QC
            uint64_t            m_viewid;           //the view#id under m_account when finish QC or consensus
            uint64_t            m_parent_height;    //height of container(like tableblock) that carry this sub-block
            uint64_t            m_parent_viewid;    //viewid of container(like tableblock) that carry this sub-block
            uint64_t            m_drand_height;     //height/round of d-rand

            xvip2_t             m_validator;        //XIP address about who issue this block or who is leader
            xvip2_t             m_auditor;          //XIP address(wild* address) about who(a cluster or a subset) should audit this block

            uint8_t             m_consensus;        //[enum_xconsensus_type][enum_xconsensus_threshold][enum_xconsensus_audit_flag]
            uint8_t             m_cryptos;          //[enum_xvchain_key_curve][enum_xvchain_sign_scheme][enum_xhash_type]
            uint16_t            __alignment_use_only__; //alignment as 8 bytes


            //each block has [header]+[input]+[output]+[input resource]+[output resource] are linked
            //when enum_xvblock_character_certify_header_only enable ,m_header_hash = hash(xvheader)
            //otherwise m_header_hash = hash(xvheader+xvinput+xvoutput) <---default mode
            std::string         m_header_hash;      //hash of to be proof for block
            std::string         m_input_root_hash;  //merkle tree' root of commands(or txs),it might be nil
            std::string         m_output_root_hash; //merkle tree' root of the executed result of cmds/txs,it might be nil
            std::string         m_justify_cert_hash;//point the block hash of locked block for unit-block,or point to m_output_root_hash of locked block for other cases depend on enum_xvblock_level

            //combine the above data together as "hash to sign" by verify_signature and audit_signature
            std::string         m_verify_signature; //Quorum-Signature of BLS/Schnorr,or PoW Proof as signature
            std::string         m_audit_signature;  //Quorum-Signature of BLS/Schnorr,or PoW Proof as signature

        private:
            std::string         m_extend_data;      //extra information as json format,e.g. a merkle proof between this cert and parent cert
            std::string         m_extend_cert;      //higher layer' qcert that certify for hash(full bin data of this cert), or comming block'cert as proof for previouse block
        };

        //once xvinput_t constructed,it not allow modify then
        class 
        xvinput_t : public xvexemodule_t, public xstatistic::xstatistic_obj_face_t
        {
            friend class xvblock_t;
        public:
            static  const std::string   name(){ return std::string("xvinput");}
            static  constexpr char const * RESOURCE_NODE_SIGN_STATISTICS     = "2";
            virtual std::string         get_obj_name() const override {return name();}
            enum{enum_obj_type = enum_xobject_type_vinput};//allow xbase create xvinput_t object from xdataobj_t::read_from()

        public:
            // xvinput_t(const std::vector<xventity_t*> & entitys,const std::string & raw_resource_data,enum_xobject_type type = enum_xobject_type_vinput);

            xvinput_t(std::vector<xventity_t*> && entitys,xstrmap_t* resource_obj,enum_xobject_type type = enum_xobject_type_vinput);
            xvinput_t(const std::vector<xventity_t*> & entitys,xstrmap_t* resource_obj,enum_xobject_type type = enum_xobject_type_vinput);
        protected:
            xvinput_t(enum_xobject_type type = enum_xobject_type_vinput);
            virtual ~xvinput_t();
        private:
            xvinput_t(const xvinput_t & other);
            xvinput_t & operator = (const xvinput_t & other);

        public:
            //caller need to cast (void*) to related ptr
            virtual void*   query_interface(const int32_t _enum_xobject_type_) override;

        public:
            //proposal usally include raw transaction & receipts. note:following methods are not thread-safe
            // virtual const std::string   get_proposal() const {return m_proposal;}
            // virtual bool                set_proposal(const std::string & proposal){m_proposal = proposal;return true;}

            //root of input which usally present a root of merkle tree for input
            virtual const std::string   get_root_hash() const {return m_root_hash;}
            virtual bool                set_root_hash(const std::string & root_hash){ m_root_hash = root_hash;return true;}

            base::xvinentity_t*         get_primary_entity() const;
            size_t                      get_action_count() const;
            virtual std::string         dump() const override;

            virtual int32_t            get_class_type() const override {return xstatistic::enum_statistic_vinput;}
        private:
            size_t get_object_size_real() const override;

        protected: //proposal ==> input ==> output
            //just carry by object at memory,not included by serialized
            // std::string  m_proposal;    //raw proposal
            std::string  m_root_hash;   //root of merkle tree constructed by input
        };

        //once xvoutput_t constructed,it not allow modify then
        class xvoutput_t : public xvexemodule_t, public xstatistic::xstatistic_obj_face_t
        {
            friend class xvblock_t;
        public:
            static  const std::string   name(){ return std::string("xvoutput");}
            virtual std::string         get_obj_name() const override {return name();}
            enum{enum_obj_type = enum_xobject_type_voutput};//allow xbase create xvoutput_t object from xdataobj_t::read_from()

            static  constexpr char const * RESOURCE_RELAY_BLOCK        = "1";            
            static  constexpr char const * RESOURCE_ACCOUNT_INDEXS     = "2";

        public:
            xvoutput_t(std::vector<xventity_t*> && entitys,enum_xobject_type type = enum_xobject_type_voutput);
            xvoutput_t(const std::vector<xventity_t*> & entitys,const std::string & raw_resource_data, enum_xobject_type type = enum_xobject_type_voutput);
            xvoutput_t(const std::vector<xventity_t*> & entitys,xstrmap_t* resource,enum_xobject_type type = enum_xobject_type_voutput);//xvqcert_t used for genreate hash for resource
        protected:
            xvoutput_t(enum_xobject_type type = enum_xobject_type_voutput);
            virtual ~xvoutput_t();
        private:
            xvoutput_t(const xvoutput_t & other);
            xvoutput_t & operator = (const xvoutput_t & other);

        public:
            //caller need to cast (void*) to related ptr
            virtual void*   query_interface(const int32_t _enum_xobject_type_) override;

            //root of input which usally present a root of merkle tree for input
            virtual const std::string   get_root_hash() const {return m_root_hash;}
            virtual bool                set_root_hash(const std::string & root_hash){ m_root_hash = root_hash;return true;}
            base::xvoutentity_t*        get_primary_entity() const;
            virtual std::string         dump() const override;

            virtual int32_t            get_class_type() const override {return xstatistic::enum_statistic_voutput;}
        private:
            size_t get_object_size_real() const override;


        protected:
            const std::string           get_binlog_hash();
            const std::string           get_state_hash();  // only can be used by xvblock_t, only light-table store state hash in output entity
            const std::string           get_output_offdata_hash() const;
            const std::string           get_binlog();
            const std::string           get_account_indexs();
        protected:
            //just carry by object at memory,not included by serialized
            std::string  m_root_hash;  //root of merkle tree constructed by input
        };

        //virtual block at high leve abstractly define and present a chain "block" that could fill by any format
        /* TOP hBFT(xthBFT) ' model & structure for each account
         ...[ vblock with this_qc]<---[ vblock with this_qc]<---[ vblock with this_qc]<---[ vblock with this_qc] ...
                /        \                 /        \              /        \                  /        \
            [last_qc ][last_block]    [last_qc ][last_block]    [last_qc ][last_block]    [last_qc ][last_block]
         */
        //vs
        /* TOP pBFT(xtpBFT))' model & structure for each account
            ...[vblock]<-------- block-hash & qc-hash  --------[vblock]<---...
                    \                                            /
                    [vblock]<-lastview#-[vblock]<-lastview#-[vblock]
        */
        //note#1:each vblock must has 3 valid part:  [header] [body] [cert],but allow empty body of block(aka,nil block)
        //note#2:each vblock has unique view#id that must increased whatever the vblock fail to get majority agreed or not

        //highest 8bit for block flags of uint16_t,total only 8 flags to define
        //note:once reset all flags to 0 ->indicate that block is no-longer valid
        enum enum_xvblock_flag
        {
            // enum_xvblock_flag_connected         = 0x0100, //block connected all the ways to genesis block or last full-block
            enum_xvblock_flag_stored            = 0x0200, //block has been stored to db/xstore fully
            enum_xvblock_flag_unpacked          = 0x0400, //unpacked container(e.g.tableblock) into multiple unit blocks
            enum_xvblock_flag_confirmed         = 0x0800, //block has delivered receipt to receiver who confirmed and finished
            enum_xvblock_flags_low4bit_mask     = 0x0F00, //mask to retrive low 4bit of block flags

            enum_xvblock_flag_authenticated     = 0x1000, //block has been certificated --block'signature has been verified
            enum_xvblock_flag_locked            = 0x2000, //block has been locked   ---block not allow to fork
            enum_xvblock_flag_committed         = 0x4000, //block has been commited ---block to allow change state of account
            // enum_xvblock_flag_executed          = 0x8000, //block has been executed ---block has been executed and changed state
            enum_xvblock_flags_high4bit_mask    = 0xF000, //mask to retrive high 4bit of block flags

            enum_xvblock_flags_mask             = 0xFF00, //mask to get all block flags
        };

        //note: xvblock must have associated xvheader_t and xvqcert_t objects
        using xvheader_ptr_t = xobject_ptr_t<base::xvheader_t>;

        class xinput_actions_cache_base {
        public:
            virtual void loop_actions(std::function<void(const base::xvaction_t & _action)> _func) const = 0;
        };

        class xvblock_excontainer_base {
        public:
            virtual void commit(base::xvblock_t* current_block) = 0;
            virtual void extract_sub_blocks(std::vector<xobject_ptr_t<xvblock_t>> & sub_blocks) const = 0;
            virtual std::shared_ptr<xinput_actions_cache_base> get_input_actions_cache() const = 0;
        };

        class xvsubblock_index_t {
        public:
            xvsubblock_index_t(std::string const& address, uint64_t height, enum_xvblock_class _class)
            : m_block_address(address), m_block_height(height), m_block_class(_class) {}
        public:
            std::string const&  get_block_address() const {return m_block_address;}
            uint64_t            get_block_height() const {return m_block_height;}
            enum_xvblock_class  get_block_class() const {return m_block_class;}
        private:
            std::string             m_block_address;
            uint64_t                m_block_height;
            enum_xvblock_class      m_block_class;
        };

        class xvblock_t : public xdataobj_t, public xstatistic::xstatistic_obj_face_t
        {
            friend class xvblockbuild_t;
            friend class xvblockstore_t;
        public:
            static  const std::string  name(){return "xvblock";}
            static  const std::string  get_header_name(){return "header";} //create_block_path()/header where stored header and cert
            static  const std::string  get_input_name() {return "input";}  //create_block_path()/body where stored body at DB
            static  const std::string  get_output_name(){return "output";} //create_block_path()/body where stored body at DB
            static  const std::string  create_header_path(const std::string & account,const uint64_t height);

        public: //create object from serialized data
            static xvblock_t*          create_block_object(const std::string  & vblock_serialized_data, bool check_input_output = true);
            static xvheader_t*         create_header_object(const std::string & vheader_serialized_data);
            static xvqcert_t*          create_qcert_object(const std::string  & vqcert_serialized_data);
            static xvinput_t*          create_input_object(bool include_resource, const std::string  & vinput_serialized_data);
            static xvoutput_t*         create_output_object(bool include_resource, const std::string & voutput_serialized_data);
            static xvbstate_t*         create_state_object(const std::string & serialized_data);

        public:
            virtual std::string        get_obj_name() const override {return name();}
            enum{enum_obj_type = enum_xobject_type_vblock};//allow xbase create xvblock_t object from xdataobj_t::read_from()
            //check whether those qcert,header,input,output etc are constist and pass test of hash
            bool  prepare_block(xvqcert_t & _vcert,xvheader_t & _vheader,xvinput_t * _vinput,xvoutput_t * _voutput);
            static  void                register_object(xcontext_t & _context); //internal use only
        public:
            xvblock_t(const xvblock_t & obj,enum_xdata_type type = (enum_xdata_type)enum_xobject_type_vblock);
        protected:
            xvblock_t(enum_xdata_type type);  //just use when seralized from db/store
            xvblock_t(xvheader_t & _vheader,xvqcert_t & _vcert,xvinput_t * _vinput,xvoutput_t * _voutput,enum_xdata_type type = (enum_xdata_type)enum_xobject_type_vblock);
            virtual ~xvblock_t();
        private:
            xvblock_t();  //just use when seralized from db/store
            xvblock_t & operator = (const xvblock_t & other);

        public:
            virtual bool                is_valid(bool deep_test = false)    const;//not verify hash of header&input&output when deep_test is alse
            virtual bool                is_deliver(bool deep_test = false)  const;//test whether has certification ready
            virtual bool                is_input_ready(bool full_check_resources = false)  const;                  //nil-block return true because it dont need input
            virtual bool                is_output_ready(bool full_check_resources = false) const;                  //nil-block return true because it dont need output
            virtual bool                is_output_offdata_ready(bool full_check_resources = false) const;
            virtual bool                is_body_and_offdata_ready(bool full_check_resources = false) const;
            virtual bool                is_execute_ready() const {return true;}//check whether ready to execute bin-log

            bool                        is_genesis_block() const;                 //test whether it is a genesis block
            virtual bool                is_equal(const xvblock_t & other)   const;//compare everyting except certification
            virtual void*               query_interface(const int32_t _enum_xobject_type_) override;//caller need to cast (void*) to related ptr

        public://header&cert get apis
            //note:m_height,m_networkid,m_account are copy from m_vheader_ptr,so PLEASE update_header() when xvheader_t changed them(rare case)
            inline  uint64_t            get_viewid()      const {return m_vqcert_ptr != nullptr ? m_vqcert_ptr->get_viewid() : 0;}
            inline  uint32_t            get_viewtoken()   const {return m_vqcert_ptr != nullptr ? m_vqcert_ptr->get_viewtoken() : 0;}
            inline  uint64_t            get_clock()       const {return m_vqcert_ptr != nullptr ? m_vqcert_ptr->get_clock() : 0;}
            inline  uint64_t            get_height()      const {return m_vheader_ptr->get_height();}
            inline  uint32_t            get_chainid()     const {return m_vheader_ptr->get_chainid();}
            inline  const std::string&  get_account()     const {return m_vheader_ptr->get_account();}
            inline  enum_xvblock_class  get_block_class() const {return m_vheader_ptr->get_block_class();}
            inline  enum_xvblock_type   get_block_type()  const {return m_vheader_ptr->get_block_type();}
            inline  enum_xvblock_level  get_block_level() const {return m_vheader_ptr->get_block_level();}
            inline  uint64_t            get_timestamp()   const {return m_vqcert_ptr->get_gmtime();}  // default timestamp is clock level gmtime, which is used for tx execute
            uint64_t                    get_second_level_gmtime() const;  // table-block has second level gmtime used for performance statistics
            
            bool                        is_fullunit()     const; // TODO(jimmy) move to xunit
            bool                        is_lightunit()    const; // TODO(jimmy) move to xunit
            bool                        is_emptyunit()    const; // TODO(jimmy) move to xunit for old unit version
            bool                        is_state_changed_unit() const; // TODO(jimmy) move to xunit

            //note:block'hash actually = cert'hash
            inline  const  std::string& get_cert_hash()   const {return m_cert_hash;}
            inline  const  std::string& get_block_hash()  const {return get_cert_hash();}
            inline  const  std::string& get_last_block_hash()   const {return m_vheader_ptr->get_last_block_hash();}
            inline  uint64_t            get_last_full_block_height() const {return m_vheader_ptr->get_last_full_block_height();}
            inline  const  std::string& get_last_full_block_hash() const{return m_vheader_ptr->get_last_full_block_hash();}

            inline uint32_t             get_block_version()       const  { return m_vheader_ptr->get_block_version();}
            inline int                  get_block_version_major() const  { return m_vheader_ptr->get_block_version_major();}
            inline int                  get_block_version_minor() const  { return m_vheader_ptr->get_block_version_minor();}
            inline int                  get_block_version_patch() const  { return m_vheader_ptr->get_block_version_patch();}

            inline  const  std::string& get_header_hash() const {return m_vqcert_ptr->get_header_hash();}
            inline  const  std::string& get_input_hash()  const {return m_vheader_ptr->get_input_hash();}
            inline  const  std::string& get_output_hash() const {return m_vheader_ptr->get_output_hash();}
            inline  const  std::string& get_input_root_hash()   const {return m_vqcert_ptr->get_input_root_hash();}
            inline  const  std::string& get_output_root_hash()  const {return m_vqcert_ptr->get_output_root_hash();}
            inline  const  std::string& get_justify_cert_hash() const {return m_vqcert_ptr->get_justify_cert_hash();}

            inline  xvblock_t*          get_prev_block()  const {return m_prev_block;}   //raw ptr of parent block of this
            inline  xvblock_t*          get_next_block()  const {return m_next_block;}   //raw ptr of child/next block of this
            inline  xvqcert_t*          get_next_next_cert()   const{return m_next_next_qcert;}//raw ptr of next->next->cert
            inline  uint64_t            get_next_next_viewid() const{return m_next_next_viewid;}//return persisted one

            inline  xvheader_t*         get_header()      const {return m_vheader_ptr;}  //raw ptr of xvheader_t
            inline  xvqcert_t *         get_cert()        const {return m_vqcert_ptr;}   //raw ptr of xvqcert_t

            const   std::string         get_header_path() const; //header include vcert part as well under get_block_path()
            const   std::string         get_input_path()  const; //path pointed to vbody at DB/disk  under get_block_path()
            const   std::string         get_output_path() const; //path pointed to vbody at DB/disk  under get_block_path()
            const   std::string         build_block_hash() const;
            const   std::string         build_header_hash() const;

            //note:container(e.g. Table,Book etc) need implement this function as they have mutiple sub blocks inside them,
            virtual bool                extract_sub_blocks(std::vector<xobject_ptr_t<xvblock_t>> & sub_blocks) {return false;}//as default it is none
            //note:container(e.g. Lightunit etc) need implement this function as they have mutiple sub txs inside them,
            virtual bool                extract_sub_txs(std::vector<xvtxindex_ptr> & sub_txs) {return false;}//as default it is none

            virtual xauto_ptr<xvblock_t> clone_block() const;
            virtual bool                close(bool force_async = true) override; //close and release this node only
            virtual std::string         dump() const override;  //just for debug purpose
            const   std::string&        dump2();  //just for debug and trace purpose with better performance
        public:
            virtual std::vector<xvsubblock_index_t> get_subblocks_index() const {return std::vector<xvsubblock_index_t>{};}

            const std::string           get_fullstate_hash() const;
            const std::string           get_account_indexs() const;
            const std::string           get_binlog_hash() const;            
            const std::string           get_full_state() const;
            const std::string           get_binlog() const;
            uint64_t                    get_block_size();
            int64_t                     get_pledge_balance_change_tgas() const;

            //only open for xvblock_t object to set them after verify singature by CA(xvcertauth_t)
            void                        set_verify_signature(const std::string & proof);
            void                        set_audit_signature(const std::string & proof);
            void                        set_extend_data(const std::string& extention);
            void                        set_extend_cert(const std::string & _cert_bin);
            bool                        merge_cert(xvqcert_t & other); //do check first before merge

            //note:it NOT mutiple-thread safe for set_block_flag(),since it may dynamic re-calculated cert'hash
            bool                        set_block_flag(enum_xvblock_flag flag); //refer enum_xvblock_flag
            bool                        check_block_flag(enum_xvblock_flag flag) const; //test whether has been setted
            int                         get_block_flags() const{ return (get_unit_flags() & 0xFF00);} //return all flags related block
            int                         reset_block_flags(const uint32_t new_flags = 0); //clean all flags related block

            //return false if hash or height not match
            bool                        reset_prev_block(xvblock_t * _new_prev_block);
            bool                        reset_next_block(xvblock_t * _new_next_block);
            //return false if hash or height not match
            void                        set_next_next_cert(xvqcert_t * next_next_vqcert_ptr);//reset ptr of next next cert

        public: //associated information about parent block(e.g. tableblock)
            inline const std::string    get_parent_account()      const {return m_parent_account;}
            inline const uint32_t       get_parent_entity_id()    const {return m_parent_entity_id;}
            inline const uint64_t       get_parent_block_height() const {return m_vqcert_ptr != nullptr ? m_vqcert_ptr->get_parent_block_height() : 0;}
            inline const uint64_t       get_parent_block_viewid() const {return m_vqcert_ptr != nullptr ? m_vqcert_ptr->get_parent_block_viewid() : 0;}
           
            bool  set_parent_block(const std::string parent_addr, uint32_t parent_entity_id);

            void  set_vote_extend_data(const std::string & vote_data);
            const std::string &  get_vote_extend_data() const;

            void  set_excontainer(std::shared_ptr<xvblock_excontainer_base> excontainer);
            std::shared_ptr<xvblock_excontainer_base> get_excontainer() const;

        public: // input&output public apis
            xvinput_t*                  load_input(std::error_code & ec)  const;//load input on-demand
            xvoutput_t*                 load_output(std::error_code & ec) const;//load output on-demand
            std::string                 query_input_resource(std::string const & key) const;
            std::string                 query_output_resource(std::string const & key) const;
            std::string                 query_output_entity(std::string const & key) const;            
            std::string                 get_output_offdata_hash() const;

            std::string const&          get_input_data() const {return m_vinput_data;}
            std::string const&          get_output_data() const {return m_voutput_data;}
            std::string const&          get_output_offdata() const {return m_output_offdata;}            
            inline bool                 has_input_data() const {return !m_vinput_data.empty();}
            inline bool                 has_output_data() const {return !m_voutput_data.empty();}
            inline bool                 has_output_offdata() const {return !m_output_offdata.empty();}
            bool                        should_has_input_data() const;
            bool                        should_has_output_data() const;
            bool                        should_has_output_offdata() const;

            bool                        set_input_data(const std::string & input_data, bool check_hash=true);
            bool                        set_output_data(const std::string & output_data, bool check_hash=true);            
            bool                        set_input_output(base::xvinput_t* _input_object, base::xvoutput_t* _output_object);
            bool                        set_output_offdata(const std::string & raw_data, bool check_hash=true);
        private: // input&output private apis
            xvinput_t*                  get_input()  const;//raw ptr of xvinput_t
            xvoutput_t*                 get_output() const;//raw ptr of xvoutput_t
            std::string const&          get_input_data_hash() const;
            std::string const&          get_output_data_hash() const;

        private:
            //generated the unique path of object(like vblock) under store-space(get_store_path()) to store data to DB
            //path like :   chainid/account/height/name
            static std::string          get_object_path(const std::string & account,uint64_t height,const std::string & name);
            void                        remove_block_flag(enum_xvblock_flag flag);  //not allow remove flag

            //only just store m_vheader_hash , m_vbody_hash and related managed-purpose information(height,account,path etc)
            virtual int32_t             do_write(xstream_t & stream) override final; //not alow change behavior anymore
            virtual int32_t             do_read(xstream_t & stream)  override final; //not alow change behavior anymore
            virtual int32_t             serialize_from(xstream_t & stream) override final;//not allow subclass change behavior
        public://still public serialize_to
            virtual int32_t             serialize_to(xstream_t & stream) override final; //not allow subclass change behavior
            //proposal usally include raw transaction & receipts. note:following methods are not thread-safe
            virtual const std::string   get_proposal() const {return m_proposal;}
            virtual bool                set_proposal(const std::string & proposal){m_proposal = proposal;return true;}
            void                        set_not_serialize_input_output(bool value);
            virtual int32_t            get_class_type() const override {return xstatistic::enum_statistic_vblock;}
        private:
            size_t get_object_size_real() const override;
        private:
            std::string                 m_cert_hash;        //hash(vqcert_bin)

            xvheader_t*                 m_vheader_ptr;      //note: it must be valid at all time
            xvqcert_t *                 m_vqcert_ptr;       //note: it must be valid at all time

            mutable xvinput_t*          m_vinput_ptr;       //note: it must be valid at all time,even a empty input
            mutable xvoutput_t*         m_voutput_ptr;      //note: it must be valid at all time,even a empty output
            std::string                 m_vinput_data;      //note: on demand load input object
            std::string                 m_voutput_data;     //note: on demand load input object
            std::string                 m_output_offdata;
            uint64_t                    m_next_next_viewid; //persist store viewid of next and next hqc

        private://not serialized to db.
            xvblock_t*                  m_prev_block;       //note: m_prev_block(aka parent)  'height must = m_block'height - 1
            xvblock_t*                  m_next_block;       //note: m_next_block(aka child)   'height must = m_block'height + 1
            //note: might have multiple blocks that point to same parent block while consensuing
            //but each block only allow have one child block.

            //note: m_next_next_qcert NOT go to persist stored
            xvqcert_t*                  m_next_next_qcert;  //temporary hold ptr of next and next hqc to proov this block as commited

        private://just using them at running and stored in sepereated place than xvblock_t.
            std::string                 m_dump_info;        //pre-print debug inforatmion and just for performance
            std::string                 m_parent_account;   //container(e.g.tableblock)'account id(refer xvaccount_t::get_xvid())
            uint32_t                    m_parent_entity_id{0};  //entity id of container(like tableblock) that carry this sub-block
            std::string                 m_vote_extend_data;
            std::shared_ptr<xvblock_excontainer_base> m_excontainer{nullptr};
            std::string                 m_proposal;    //raw proposal
            bool                        m_not_serialize_input_output{false};
            mutable xspinlock_t         m_spin_lock;
        };
        using xvblock_ptr_t = xobject_ptr_t<base::xvblock_t>;

        struct less_by_block_height
        {
            bool operator()(const base::xvblock_t * front,const base::xvblock_t * back)
            {
                if(nullptr == front)
                    return true;

                if(nullptr == back)
                    return false;

                return (front->get_height() < back->get_height());
            }
            bool operator()(const base::xvblock_t & front,const base::xvblock_t & back)
            {
                return (front.get_height() < back.get_height());
            }
        };

        struct less_by_block_viewid
        {
            bool operator()(const base::xvblock_t * front,const base::xvblock_t * back)
            {
                if(nullptr == front)
                    return true;

                if(nullptr == back)
                    return false;

                return (front->get_viewid() < back->get_viewid());
            }

            bool operator()(const base::xvblock_t & front,const base::xvblock_t & back)
            {
                return (front.get_viewid() < back.get_viewid());
            }
        };
        struct greater_by_block_viewid
        {
            bool operator()(const base::xvblock_t * front, const base::xvblock_t * back)
            {
                if(nullptr == front)
                    return false;

                if(nullptr == back)
                    return true;

                return (front->get_viewid() > back->get_viewid());
            }

            bool operator()(const base::xvblock_t & front, const base::xvblock_t & back)
            {
                return (front.get_viewid() > back.get_viewid());
            }
        };
        
    }//end of namespace of base

}//end of namespace top
