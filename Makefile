include package.conf

NAME = PARSEGRAN
FPARSER = fparser27/fparser
OBJS = $(NAME).o $(FPARSER).o
CMIXOBJS += $(PROFILE_O)
CXXFLAGS += -I. -Wall 
PROGS = $(NAME) lib$(NAME).so 

all: lib$(NAME).so

standalone: $(NAME)

lib$(NAME).so: $(OBJS) $(GENLIB)
	$(CXX) $(SHARED_LDFLAGS) -o $@ $(OBJS) $(GENLIB) $(SYSLIBS)


$(NAME): $(OBJS) $(CMIXOBJS)
	$(CXX) -o $@ $(OBJS) $(CMIXOBJS) $(LDFLAGS)



$(OBJS): $(INSTRUMENT_H) $(NAME).h $(FPARSER).hh


clean:
	$(RM) $(OBJS) $(PROGS)

