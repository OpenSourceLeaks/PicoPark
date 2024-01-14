/*!
 * リソース
 * @author Miyake Shunsuke
 * @since 2011.12.15
 */

// コンマ
#if TB_RESOURCE_ARGS_MAX > 0
    #define TB_RESOURCE_COMMA ,
#else
    #define TB_RESOURCE_COMMA
#endif

namespace toybox
{

// リソース
template<typename T TB_RESOURCE_COMMA TB_RESOURCE_TEMPLATE_USERARGS>
class TbResource : public TbResourceBase
{
    friend class TbResourceManager;
public:

    // 自分自身
    typedef TbResource<T TB_RESOURCE_COMMA TB_RESOURCE_USERARGS> Self;

public: // 静的関数

    // タイプID取得
    static TbTypeId GetTypeIdStatic() {
        return TbGetTypeId<Self>();
    }

public: // 公開関数

    // リソース取得
    T* GerResource() {
        return m_Resource;
    }

    // リソース取得
    const T* GetResource() const {
        return m_Resource;
    }

public: // オーバーライド

    // タイプID取得
    virtual TbTypeId GetTypeId() const {
        return TbGetTypeId<Self>();
    }

protected:

    // コンストラクタ
    TbResource( const char* fileName , TbFileSyncMode syncMode , TbFilePathId pathId );

    // デストラクタ
    virtual ~TbResource();

private:

    T*    m_Resource;                   //!< リソース
    TB_RESOURCE_DECLMEMBER_USERARGS;    //!< ユーザーパラメータ

};

}
