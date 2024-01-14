/*!
 * オブサーバー
 * @author Miyake_Shunsuke
 * @since 2012.05.01
 */

#ifndef _INCLUDED_TB_OBSERVER_H_
#define _INCLUDED_TB_OBSERVER_H_

#define TB_DEFINE_OBSEVER_BEGIN(Name)

class TbObserver
{
public:
    TbObserver();
    ~TbObserver();
private:
    friend class TbSubject;
    TbSubject*  m_Subject;
    TbObserver* m_Prev;
    TbObserver* m_Next;
};

class TbSubject
{
public:
    void AddObserver();
    void RemoveObserver();
private:
    TbObserver* m_ObserverTop;
};

#endif
