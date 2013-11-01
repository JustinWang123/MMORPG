#ifndef BASE_VIEW_H
#define BASE_VIEW_H



class BaseView {
public:
	virtual void			Update()=0;
	virtual void			Draw()=0;
};

#endif