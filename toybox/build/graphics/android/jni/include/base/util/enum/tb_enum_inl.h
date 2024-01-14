namespace toybox
{

// 比較演算子(==)
template <typename T,typename EnumType,typename InnerType>
TB_FORCE_INLINE TbBool operator==(const T& src , const TbEnum<EnumType,InnerType>& dst)
{
    return src == dst.m_Value;
}

// 比較演算子(!=)
template <typename T,typename EnumType,typename InnerType>
TB_FORCE_INLINE TbBool operator!=(const T& src , const TbEnum<EnumType,InnerType>& dst)
{
    return src != dst.m_Value;
}

}
