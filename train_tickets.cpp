#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _ticket_book_record
{
   int booked[1];
} ticket_book_record_t;

typedef struct _train_way
{
   int seats;
   int stations;
   ticket_book_record_t * records;
   int prices[1];
} train_way_t;

typedef struct _ticket
{
   train_way_t * way;
   int id;
   int seat;
   int first_station;
   int last_station;
   int serial_number;
} ticket_t;

int min( int a, int b )
{
   return a < b ? a : b;
}

// return an array contains ticket_book_record_t for all seats
ticket_book_record_t * create_ticket_book_record( int seats, int stations )
{
   if ( !seats || stations < 2 )
      return NULL;

   int size_of_book_record = sizeof(ticket_book_record_t) + sizeof( int ) * ( stations - 2 );
   ticket_book_record_t * records = (ticket_book_record_t *)malloc( size_of_book_record * seats );

   if ( !records )
      return NULL;

   for ( int i = 0; i < seats; i++ )
   {
      for ( int j = 0; j < stations - 1; j++ )
      {
         // set booked to 0 to mark it as none booked
         records[i].booked[j] = 0;
      }
   }

   return records;
}

// random generate prices of the train way
train_way_t * randome_create_train_way( int seats, int stations )
{
   if ( stations < 2 || seats == 0 )
      return NULL;

   train_way_t * way = (train_way_t *)malloc( sizeof(train_way_t) + sizeof( int ) * ( stations - 2 ) );

   if ( !way )
      return NULL;

   way->seats = seats;
   way->stations = stations;

   for ( int i = 0; i < stations - 1; i++ )
   {
      way->prices[i] = rand() % 100;
   }

   way->records = create_ticket_book_record( seats, stations );

   return way;
}

void destroy_train_way( train_way_t * way )
{
   if ( !way )
      return;

   if ( way->records )
      free( way->records );

   free( way );
}

// book ticket
bool book_ticket( ticket_t * ticket_req, train_way_t * way, ticket_book_record_t * records )
{
   // check ticket_req is valid or not
   if ( !( ticket_req->way == way && ticket_req->first_station >= 0 && ticket_req->last_station < way->stations ) )
      return false;

   // find free seat
   for ( int i = 0; i < way->seats; i++ )
   {
      int j = ticket_req->first_station;

      for ( ; j < ticket_req->last_station; j++ )
      {
         if ( records[i].booked[j] != 0 )
            break;
      }

      // found a free ticket
      if ( j == ticket_req->last_station )
      {
         // book it
         for ( j = ticket_req->first_station; j < ticket_req->last_station; j++ )
         {
            records[i].booked[j] = ticket_req->id;
         }

         ticket_req->seat = i;

         return true;
      }
   }

   return false;
}

void refund_ticket( ticket_t * ticket, train_way_t * way )
{
   if ( !( ticket->way == way && ticket->first_station >= 0 && ticket->last_station < way->stations ) )
      return; // invalid ticket

   for ( int j = ticket->first_station; j < ticket->last_station; j++ )
      way->records[ticket->seat].booked[j] = 0;

   ticket->seat = -1;
}

int price_ticket( ticket_t * ticket )
{
   if ( !ticket || !ticket->way )
      return 0; // invalid ticket worth nothing

   // sum the price together
   int price = 0;
   
   for ( int j = ticket->first_station; j < min(ticket->last_station, ticket->way->stations); j++ )
   {
      price += ticket->way->prices[j];
   }

   return price;
}

void pay_ticket( ticket_t * ticket )
{
   if ( !ticket || !ticket->way )
      return;

   // calculate price of ticket
   printf( "pay ticket %p with %d yuan\n", ticket, price_ticket( ticket ) );

   printf( "success ticket serial number is %d\n", ticket->serial_number );
}

bool valid_ticket( ticket_t * ticket, train_way_t * way, ticket_book_record_t * records )
{
   if ( ticket->way != way )
      return false;

   for ( int i = ticket->first_station; i < min(ticket->last_station, way->stations); i++ )
   {
      if ( records[ticket->seat].booked[i] != ticket->id )
         return false;
   }

   return true;
}

bool load_train_way( train_way_t ** way, const char * database )
{
   return false;
}

void save_train_way( train_way_t * way, const char * database )
{
}

void usage()
{
}

int main( int argc, char * argv[] )
{
   train_way_t * way = NULL;

   // import train way, a train way is composes of stations and ticket prices of neighbour stations
   for ( int i = 1; i < argc; i += 2 )
   {
      if ( !strcmp( "--create-train-way", argv[i] ) )
      {
         // the longest train way in this example cannot exceed 20 stations
         // the maximum of seats cannot exceed 1000
         way = randome_create_train_way( rand() % 1000, rand() % 20 );
      }
      else if ( !strcmp( "--load-train-way", argv[i] ) )
      {
         if ( !load_train_way( &way, argv[i + 1] ) )
         {
            fprintf( stderr, "cannot load train way from %s\n", argv[i] );
            return -1;
         }
      }
      else if ( !strcmp( "--book-ticket", argv[i + 1] ) )
      {
      }
      else if ( !strcmp( "--pay-ticket", argv[i + 1] ) )
      {
      }
      else if ( !strcmp( "--refund-ticket", argv[i + 1] ) )
      {
      }
      else
      {
         fprintf( stderr, "unknown command %s", argv[i] );
         usage();
         return -1;
      }
   }

   save_train_way( way, "train-way.db" );
   destroy_train_way( way );
   return 0; 
}
